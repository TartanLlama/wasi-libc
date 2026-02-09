#include "pthread_impl.h"
#include "syscall.h"

int pthread_attr_setinheritsched(pthread_attr_t *a, int inherit)
{
	#ifdef __WASI_THREADS_COOPERATIVE__
	return ENOTSUP;
	#else
	if (inherit > 1U) return EINVAL;
	a->_a_sched = inherit;
	return 0;
	#endif
}
