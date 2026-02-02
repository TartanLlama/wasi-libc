#ifndef LOCK_H
#define LOCK_H

#if defined(__wasip3__)
#include "pthread_impl.h"
struct __coop_lock {
	int owner;  // tid of owning thread, or 0 if unlocked
	struct __waitlist_node *waiters;
};

typedef struct __coop_lock __lock_t;
typedef struct __coop_lock *__lock_ptr_t;
#define __COOP_LOCK_INIT {0, NULL}
#define __LOCK_INIT __COOP_LOCK_INIT

hidden void __lock(struct __coop_lock *lock);
hidden void __unlock(struct __coop_lock *lock);
#define LOCK(x) __lock(x)
#define UNLOCK(x) __unlock(x)
#elif defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
typedef volatile int __lock_t;
typedef volatile int *__lock_ptr_t;
#define __LOCK_INIT {0}

hidden void __lock(volatile int *);
hidden void __unlock(volatile int *);
#define LOCK(x) __lock(x)
#define UNLOCK(x) __unlock(x)
#else
// No locking needed.
#define LOCK(x) ((void)0)
#define UNLOCK(x) ((void)0)
#endif

#endif
