#include "pthread_impl.h"

#ifdef __wasip3__
int __pthread_rwlock_trywrlock(pthread_rwlock_t *rw)
{
	int tid = wasip3_thread_index();
	
	/* Check for deadlock: trying to write-lock already owned write lock */
	if (rw->_rw_lock == -tid) {
		return EDEADLK;
	}
	
	/* Try to acquire write lock */
	if (rw->_rw_lock != 0) {
		return EBUSY;
	}
	
	rw->_rw_lock = -tid;
	return 0;
}
#else
int __pthread_rwlock_trywrlock(pthread_rwlock_t *rw)
{
	if (a_cas(&rw->_rw_lock, 0, 0x7fffffff)) return EBUSY;
	return 0;
}
#endif

weak_alias(__pthread_rwlock_trywrlock, pthread_rwlock_trywrlock);
