#include "pthread_impl.h"

#ifdef __WASI_THREADS_COOPERATIVE__
int __pthread_rwlock_tryrdlock(pthread_rwlock_t *rw)
{
	/* If no writer holds the lock, increment reader count */
	if (rw->_rw_lock >= 0) {
		rw->_rw_lock++;
		return 0;
	}
	return EBUSY;
}
#else
int __pthread_rwlock_tryrdlock(pthread_rwlock_t *rw)
{
	int val, cnt;
	do {
		val = rw->_rw_lock;
		cnt = val & 0x7fffffff;
		if (cnt == 0x7fffffff) return EBUSY;
		if (cnt == 0x7ffffffe) return EAGAIN;
	} while (a_cas(&rw->_rw_lock, val, val+1) != val);
	return 0;
}
#endif

weak_alias(__pthread_rwlock_tryrdlock, pthread_rwlock_tryrdlock);
