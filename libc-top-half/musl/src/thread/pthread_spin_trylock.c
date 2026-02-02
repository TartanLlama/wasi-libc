#include "pthread_impl.h"
#include <errno.h>

int pthread_spin_trylock(pthread_spinlock_t *s)
{
#ifdef __wasip3__
	if (*s) return EBUSY;
	*s = EBUSY;
	return 0;
#else
	return a_cas(s, 0, EBUSY);
#endif
}
