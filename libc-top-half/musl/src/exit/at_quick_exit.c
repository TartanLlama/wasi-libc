#include <stdlib.h>
#include "libc.h"
#include "lock.h"
#include "fork_impl.h"

#define COUNT 32

static void (*funcs[COUNT])(void);
static int count;
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
#ifdef __wasip3__
#include "pthread_impl.h"
static struct __coop_lock lock[1] = {__COOP_LOCK_INIT};
static struct __coop_lock *const __at_quick_exit_lockptr = lock;
#else
static volatile int lock[1];
volatile int *const __at_quick_exit_lockptr = lock;
#endif
#endif

void __funcs_on_quick_exit()
{
	void (*func)(void);
	LOCK(lock);
	while (count > 0) {
		func = funcs[--count];
		UNLOCK(lock);
		func();
		LOCK(lock);
	}
}

int at_quick_exit(void (*func)(void))
{
	int r = 0;
	LOCK(lock);
	if (count == 32) r = -1;
	else funcs[count++] = func;
	UNLOCK(lock);
	return r;
}
