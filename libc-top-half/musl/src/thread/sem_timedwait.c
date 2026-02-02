#include <semaphore.h>
#include <limits.h>
#include "pthread_impl.h"

#ifdef __wasip3__

int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict at)
{
	pthread_testcancel();
	
	/* Fast path: if permit is available, take it regardless of timeout */
	if (sem->__count > 0) {
		sem->__count--;
		return 0;
	}
	
	/* Would need to block, but timeouts not supported in cooperative threading.
	 * There's no way to interrupt a suspended thread after a timeout. */
	if (at) {
		errno = ENOSYS;
		return -1;
	}
	
	/* Loop until we successfully acquire a permit */
	while (sem->__count == 0) {
		/* No permits available, wait on the waitlist */
		__waitlist_wait_on(&sem->__waiters);
		/* After waking, loop back to recheck - another thread
		 * might have taken the permit before we ran. */
	}
	
	/* Permit available, take it */
	sem->__count--;
	return 0;
}

#else

static void cleanup(void *p)
{
	a_dec(p);
}

int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict at)
{
	pthread_testcancel();

	if (!sem_trywait(sem)) return 0;

	int spins = 100;
	while (spins-- && !(sem->__val[0] & SEM_VALUE_MAX) && !sem->__val[1])
		a_spin();

	while (sem_trywait(sem)) {
		int r, priv = sem->__val[2];
		a_inc(sem->__val+1);
		a_cas(sem->__val, 0, 0x80000000);
		pthread_cleanup_push(cleanup, (void *)(sem->__val+1));
		r = __timedwait_cp(sem->__val, 0x80000000, CLOCK_REALTIME, at, priv);
		pthread_cleanup_pop(1);
		if (r) {
			errno = r;
			return -1;
		}
	}
	return 0;
}

#endif
