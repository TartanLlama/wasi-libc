#include "pthread_impl.h"

#ifdef __wasip3__

int __pthread_mutex_unlock(pthread_mutex_t *m)
{
	int tid = wasip3_thread_index();
	int type = m->_m_type & 15;
	
	/* Check ownership */
	if (m->_m_lock != tid) {
		if (type == PTHREAD_MUTEX_ERRORCHECK || type == PTHREAD_MUTEX_RECURSIVE) {
			return EPERM;
		}
		/* Normal mutex: undefined behavior */
		__builtin_trap();
	}
	
	/* Handle recursive unlock */
	if (type == PTHREAD_MUTEX_RECURSIVE && m->_m_count > 1) {
		m->_m_count--;
		return 0;
	}
	
	/* Unlock */
	m->_m_lock = 0;
	m->_m_count = 0;
	__waitlist_wake_one(&m->_m_waiters);
	return 0;
}

#else

int __pthread_mutex_unlock(pthread_mutex_t *m)
{
	pthread_t self;
	int waiters = m->_m_waiters;
	int cont;
	int type = m->_m_type & 15;
	int priv = (m->_m_type & 128) ^ 128;
	int new = 0;
	int old;

	if (type != PTHREAD_MUTEX_NORMAL) {
		self = __pthread_self();
		old = m->_m_lock;
		int own = old & 0x3fffffff;
		if (own != self->tid)
			return EPERM;
		if ((type&3) == PTHREAD_MUTEX_RECURSIVE && m->_m_count)
			return m->_m_count--, 0;
		if ((type&4) && (old&0x40000000))
			new = 0x7fffffff;
		if (!priv) {
			self->robust_list.pending = &m->_m_next;
#ifdef __wasilibc_unmodified_upstream
			__vm_lock();
#endif
		}
		volatile void *prev = m->_m_prev;
		volatile void *next = m->_m_next;
		*(volatile void *volatile *)prev = next;
		if (next != &self->robust_list.head) *(volatile void *volatile *)
			((char *)next - sizeof(void *)) = prev;
	}
#ifdef __wasilibc_unmodified_upstream
	if (type&8) {
		if (old<0 || a_cas(&m->_m_lock, old, new)!=old) {
			if (new) a_store(&m->_m_waiters, -1);
			__syscall(SYS_futex, &m->_m_lock, FUTEX_UNLOCK_PI|priv);
		}
		cont = 0;
		waiters = 0;
	} else {
		cont = a_swap(&m->_m_lock, new);
	}
#else
		cont = a_swap(&m->_m_lock, new);
#endif
	if (type != PTHREAD_MUTEX_NORMAL && !priv) {
		self->robust_list.pending = 0;
#ifdef __wasilibc_unmodified_upstream
		__vm_unlock();
#endif
	}
	if (waiters || cont<0)
		__wake(&m->_m_lock, 1, priv);
	return 0;
}

#endif

weak_alias(__pthread_mutex_unlock, pthread_mutex_unlock);
