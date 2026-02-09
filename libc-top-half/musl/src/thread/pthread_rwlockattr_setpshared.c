#include "pthread_impl.h"

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *a, int pshared)
{
#ifdef __wasip3__
	return pshared == PTHREAD_PROCESS_PRIVATE ? 0 : ENOTSUP;
#else
	if (pshared > 1U) return EINVAL;
	a->__attr[0] = pshared;
	return 0;
#endif
}
