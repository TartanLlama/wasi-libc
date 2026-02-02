#include "pthread_impl.h"

#ifdef __wasip3__

int __pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rw, const struct timespec *restrict at)
{
	/* Fast path: if no readers or writers, acquire write lock */
	if (rw->_rw_lock == 0) {
		rw->_rw_lock = -1;  /* -1 indicates write lock */
		return 0;
	}
	
	/* Would need to wait, but timeouts not supported */
	if (at) {
		errno = ENOSYS;
		return -1;
	}
	
	/* Wait until no readers and no writers */
	while (rw->_rw_lock != 0) {
		__waitlist_wait_on(&rw->_rw_waiters);
	}
	
	/* Acquired, mark as write-locked */
	rw->_rw_lock = -1;
	return 0;
}

#else

int __pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rw, const struct timespec *restrict at)
{
	int r, t;

	r = pthread_rwlock_trywrlock(rw);
	if (r != EBUSY) return r;
	
	int spins = 100;
	while (spins-- && rw->_rw_lock && !rw->_rw_waiters) a_spin();

	while ((r=__pthread_rwlock_trywrlock(rw))==EBUSY) {
		if (!(r=rw->_rw_lock)) continue;
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
