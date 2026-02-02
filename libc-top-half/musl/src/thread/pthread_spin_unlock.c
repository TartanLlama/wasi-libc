#include "pthread_impl.h"

int pthread_spin_unlock(pthread_spinlock_t *s)
{
#ifdef __wasip3__
	*s = 0;
	return 0;
#else
	a_store(s, 0);
	return 0;
#endif
}
