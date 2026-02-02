#define _GNU_SOURCE
#include "pthread_impl.h"
#ifdef __wasilibc_unmodified_upstream
#include <sys/mman.h>
#endif

static void dummy1(pthread_t t)
{
}
weak_alias(dummy1, __tl_sync);

static int __pthread_timedjoin_np(pthread_t t, void **res, const struct timespec *at)
{
#ifdef __wasip3__
	/* If already exited, just return the result */
	if (t->detach_state == DT_EXITED) {
		if (res) *res = t->result;
		if (t->map_base) free(t->map_base);
		return 0;
	}
	
	/* Timeouts not supported for cooperative threading */
	if (at) {
		return ETIMEDOUT;
	}
	
	__waitlist_wait_on(&t->joiner_waiters);
	if (res) *res = t->result;
	if (t->map_base) free(t->map_base);
	return 0;
#else
	int state, cs, r = 0;
	__pthread_testcancel();
	__pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	if (cs == PTHREAD_CANCEL_ENABLE) __pthread_setcancelstate(cs, 0);
	while ((state = t->detach_state) && r != ETIMEDOUT && r != EINVAL) {
		if (state >= DT_DETACHED) a_crash();
		r = __timedwait_cp(&t->detach_state, state, CLOCK_REALTIME, at, 1);
	}
	__pthread_setcancelstate(cs, 0);
	if (r == ETIMEDOUT || r == EINVAL) return r;
	__tl_sync(t);
	if (res) *res = t->result;
#ifdef __wasilibc_unmodified_upstream
	if (t->map_base) __munmap(t->map_base, t->map_size);
#else
	if (t->map_base) free(t->map_base);
#endif
	return 0;
#endif
}

int __pthread_join(pthread_t t, void **res)
{
	return __pthread_timedjoin_np(t, res, 0);
}

static int __pthread_tryjoin_np(pthread_t t, void **res)
{
	return t->detach_state==DT_JOINABLE ? EBUSY : __pthread_join(t, res);
}

weak_alias(__pthread_tryjoin_np, pthread_tryjoin_np);
weak_alias(__pthread_timedjoin_np, pthread_timedjoin_np);
weak_alias(__pthread_join, pthread_join);
