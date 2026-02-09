#include "pthread_impl.h"

int pthread_mutexattr_setpshared(pthread_mutexattr_t *a, int pshared)
{
#ifdef __wasip3__
	return pshared == PTHREAD_PROCESS_PRIVATE ? 0 : ENOTSUP;
#else
	if (pshared > 1U) return EINVAL;
	a->__attr &= ~128U;
	a->__attr |= pshared<<7;
	return 0;
#endif
}
