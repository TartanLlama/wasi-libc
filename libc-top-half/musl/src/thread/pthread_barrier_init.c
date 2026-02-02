#include "pthread_impl.h"

int pthread_barrier_init(pthread_barrier_t *restrict b, const pthread_barrierattr_t *restrict a, unsigned count)
{
#ifdef __wasip3__
	if (count == 0 || count > INT_MAX) return EINVAL;
	*b = (pthread_barrier_t){ ._b_limit = count, ._b_count = 0, ._b_waiters = 0 };
	return 0;
#else
	if (count-1 > INT_MAX-1) return EINVAL;
	*b = (pthread_barrier_t){ ._b_limit = count-1 | (a?a->__attr:0) };
	return 0;
#endif
}
