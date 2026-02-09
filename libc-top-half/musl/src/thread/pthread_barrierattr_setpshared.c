#include "pthread_impl.h"

int pthread_barrierattr_setpshared(pthread_barrierattr_t *a, int pshared)
{
#ifdef __wasip3__
	return pshared == PTHREAD_PROCESS_PRIVATE ? 0 : ENOTSUP;
#else
	if (pshared > 1U) return EINVAL;
	a->__attr = pshared ? INT_MIN : 0;
	return 0;
#endif	
}
