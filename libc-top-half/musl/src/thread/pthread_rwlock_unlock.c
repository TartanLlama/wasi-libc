#include "pthread_impl.h"

#ifdef __wasip3__

int __pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
	if (rw->_rw_lock > 0) {
		/* Releasing read lock */
		rw->_rw_lock--;
		if (rw->_rw_lock == 0) {
			/* Last reader, wake waiting writers */
			__waitlist_wake_all(&rw->_rw_waiters);
		}
	} else if (rw->_rw_lock < 0) {
		/* Releasing write lock (stored as -tid) */
		rw->_rw_lock = 0;
		/* Wake all waiters (readers and writers compete) */
		__waitlist_wake_all(&rw->_rw_waiters);
	} else {
		/* Unlocking when not locked - trap */
		__builtin_trap();
	}
	
	return 0;
}

#else

int __pthread_rwlock_unlock(pthread_rwlock_t *rw)
{
	int val, cnt, waiters, new, priv = rw->_rw_shared^128;

	do {
		val = rw->_rw_lock;
		cnt = val & 0x7fffffff;
		waiters = rw->_rw_waiters;
		new = (cnt == 0x7fffffff || cnt == 1) ? 0 : val-1;
	} while (a_cas(&rw->_rw_lock, val, new) != val);

	if (!new && (waiters || val<0))
		__wake(&rw->_rw_lock, cnt, priv);

	return 0;
}

#endif

weak_alias(__pthread_rwlock_unlock, pthread_rwlock_unlock);
