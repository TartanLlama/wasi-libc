#include "pthread_impl.h"

#ifdef __wasip3__

int __pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rw, const struct timespec *restrict at)
{
	/* Fast path: if no writer, increment readers */
	if (rw->_rw_lock >= 0) {
		rw->_rw_lock++;
		return 0;
	}
	
	/* Would need to wait for writer, but timeouts not supported */
	if (at) {
		errno = ENOSYS;
		return -1;
	}
	
	/* Wait until no writer holds the lock */
	while (rw->_rw_lock < 0) {
		__waitlist_wait_on(&rw->_rw_waiters);
	}
	
	/* Acquired, increment reader count */
	rw->_rw_lock++;
	return 0;
}

#else

int __pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rw, const struct timespec *restrict at)
{
	int r, t;

	r = pthread_rwlock_tryrdlock(rw);
	if (r != EBUSY) return r;
	
	int spins = 100;
	while (spins-- && rw->_rw_lock && !rw->_rw_waiters) a_spin();

	while ((r=__pthread_rwlock_tryrdlock(rw))==EBUSY) {
		if (!(r=rw->_rw_lock) || (r&0x7fffffff)!=0x7fffffff) continue;
		t = r | 0x80000000;
		a_inc(&rw->_rw_waiters);
		a_cas(&rw->_rw_lock, r, t);
		r = __timedwait(&rw->_rw_lock, t, CLOCK_REALTIME, at, rw->_rw_shared^128);
		a_dec(&rw->_rw_waiters);
		if (r && r != EINTR) return r;
	}
	return r;
}

#endif

weak_alias(__pthread_rwlock_timedrdlock, pthread_rwlock_timedrdlock);
