#include "pthread_impl.h"
#include <errno.h>

int pthread_spin_lock(pthread_spinlock_t *s)
{
#ifdef __wasip3__
	/* WASIP3: Can't spin-wait with cooperative threading */
	if (*s) return EDEADLK;
	*s = EBUSY;
	return 0;
#else
	while (*(volatile int *)s || a_cas(s, 0, EBUSY)) a_spin();
	return 0;
#endif
}
