#include "pthread_impl.h"

#ifdef __wasip3__

int __pthread_mutex_lock(pthread_mutex_t *m)
{
	int tid = wasip3_thread_index();
	int type = m->_m_type & 15;
	
	/* Check for recursive lock */
	if (m->_m_lock == tid) {
		if (type == PTHREAD_MUTEX_RECURSIVE) {
			m->_m_count++;
			return 0;
		}
		if (type == PTHREAD_MUTEX_ERRORCHECK) {
			return EDEADLK;
		}
		/* Normal mutex: deadlock */
		__builtin_trap();
	}
	
	/* Wait until unlocked */
	while (m->_m_lock != 0) {
		__waitlist_wait_on(&m->_m_waiters);
	}
	
	m->_m_lock = tid;
	m->_m_count = 1;
	return 0;
}

#else

int __pthread_mutex_lock(pthread_mutex_t *m)
{
	if ((m->_m_type&15) == PTHREAD_MUTEX_NORMAL
	    && !a_cas(&m->_m_lock, 0, EBUSY))
		return 0;

	return __pthread_mutex_timedlock(m, 0);
}

#endif

weak_alias(__pthread_mutex_lock, pthread_mutex_lock);
