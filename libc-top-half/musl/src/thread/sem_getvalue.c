#include <semaphore.h>
#include <limits.h>

int sem_getvalue(sem_t *restrict sem, int *restrict valp)
{
	#ifdef __wasip3__
	*valp = sem->__count;
	/* No need to & with SEM_VALUE_MAX; non-wasip3 uses 
	the high bit of __val[0] for other purposes */
	#else
	int val = sem->__val[0];
	*valp = val & SEM_VALUE_MAX;
	#endif
	return 0;
}
