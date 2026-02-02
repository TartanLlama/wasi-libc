#include <semaphore.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>

int sem_init(sem_t *sem, int pshared, unsigned value)
{
	if (value > SEM_VALUE_MAX) {
		errno = EINVAL;
		return -1;
	}
#ifdef __wasip3__
	sem->__count = value;
	sem->__waiters = NULL;
#else
	sem->__val[0] = value;
	sem->__val[1] = 0;
	sem->__val[2] = pshared ? 0 : 128;
#endif
	return 0;
}
