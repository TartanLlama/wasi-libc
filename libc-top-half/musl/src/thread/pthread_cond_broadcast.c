#include "pthread_impl.h"

#ifdef __wasip3__

int pthread_cond_broadcast(pthread_cond_t *c)
{
	__waitlist_wake_all(&c->_c_waiters);
	return 0;
}

#else

int pthread_cond_broadcast(pthread_cond_t *c)
{
	if (!c->_c_shared) return __private_cond_signal(c, -1);
	if (!c->_c_waiters) return 0;
	a_inc(&c->_c_seq);
	__wake(&c->_c_seq, -1, 0);
	return 0;
}

#endif
