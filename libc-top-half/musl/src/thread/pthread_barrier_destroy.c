#include "pthread_impl.h"

int pthread_barrier_destroy(pthread_barrier_t *b)
{
#ifdef __wasip3__
	/* No cleanup needed for WASIP3 barriers */
	return 0;
#else
	if (b->_b_limit < 0) {
		if (b->_b_lock) {
			int v;
			a_or(&b->_b_lock, INT_MIN);
			while ((v = b->_b_lock) & INT_MAX)
				__wait(&b->_b_lock, 0, v, 0);
		}
#ifdef __wasilibc_unmodified_upstream /* WASI does not understand processes or locking between them. */
		__vm_wait();
#endif
	}
	return 0;
#endif
}
