#include "pthread_impl.h"
#include <threads.h>

static int __pthread_detach(pthread_t t)
{
#if defined(__wasip3__)
	int state = t->detach_state;
	
	/* Already detached */
	if (state == DT_DETACHED) {
		return EINVAL;
	}
	
	/* Thread already exited or is exiting - join/cleanup */
	if (state == DT_EXITING || state == DT_EXITED) {
		__pthread_join(t, 0);
	}

	t->detach_state = DT_DETACHED;
	
#else
	/* If the cas fails, detach state is either already-detached
	 * or exiting/exited, and pthread_join will trap or cleanup. */
	if (a_cas(&t->detach_state, DT_JOINABLE, DT_DETACHED) != DT_JOINABLE) {
		int cs;
		__pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
		__pthread_join(t, 0);
		__pthread_setcancelstate(cs, 0);
	}
#endif
	return 0;
}

weak_alias(__pthread_detach, pthread_detach);
weak_alias(__pthread_detach, thrd_detach);
