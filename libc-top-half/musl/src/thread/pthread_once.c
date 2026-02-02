#include "pthread_impl.h"

#ifdef __wasip3__

/* WASIP3: Simplified cooperative implementation
 * State: 0 = not run, 1 = running, 2 = completed */

static struct {
	pthread_once_t *control;
	struct __waitlist_node *waiters;
} once_state;

int __pthread_once(pthread_once_t *control, void (*init)(void))
{
	/* Fast path: already completed */
	if (*control == 2) {
		return 0;
	}
	
	/* Try to become the initializer */
	if (*control == 0) {
		*control = 1;
		once_state.control = control;
		
		/* init may do something that blocks, in which case other threads
		 * may try to run pthread_once on the same control. They will
		 * see state 1 and wait on our waitlist. */
		init();
		
		*control = 2;
		__waitlist_wake_all(&once_state.waiters);
		return 0;
	}
	
	/* Another thread is initializing, wait for completion */
	while (*control == 1) {
		__waitlist_wait_on(&once_state.waiters);
	}
	
	return 0;
}

#else

static void undo(void *control)
{
	/* Wake all waiters, since the waiter status is lost when
	 * resetting control to the initial state. */
	if (a_swap(control, 0) == 3)
		__wake(control, -1, 1);
}

hidden int __pthread_once_full(pthread_once_t *control, void (*init)(void))
{
	/* Try to enter initializing state. Four possibilities:
	 *  0 - we're the first or the other cancelled; run init
	 *  1 - another thread is running init; wait
	 *  2 - another thread finished running init; just return
	 *  3 - another thread is running init, waiters present; wait */

	for (;;) switch (a_cas(control, 0, 1)) {
	case 0:
		pthread_cleanup_push(undo, control);
		init();
		pthread_cleanup_pop(0);

		if (a_swap(control, 2) == 3)
			__wake(control, -1, 1);
		return 0;
	case 1:
		/* If this fails, so will __wait. */
		a_cas(control, 1, 3);
	case 3:
		__wait(control, 0, 3, 1);
		continue;
	case 2:
		return 0;
	}
}

int __pthread_once(pthread_once_t *control, void (*init)(void))
{
	/* Return immediately if init finished before, but ensure that
	 * effects of the init routine are visible to the caller. */
	if (*(volatile int *)control == 2) {
		a_barrier();
		return 0;
	}
	return __pthread_once_full(control, init);
}

#endif

weak_alias(__pthread_once, pthread_once);
