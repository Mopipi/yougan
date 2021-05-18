#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include <atomic>

#include "define.h"

#define __sync_fetch_and_add(ptr, v) InterlockedExchangeAdd((long*)ptr, v)
#define __sync_add_and_fetch(ptr, v) (__sync_fetch_and_add(ptr, v) + v)

#define __sync_fetch_and_sub(ptr, v) InterlockedExchangeAdd((long*)ptr, (-v))
#define  __sync_sub_and_fetch(ptr, v) (__sync_fetch_and_sub((long*)ptr, v) -(v))

#define __sync_lock_test_and_set(ptr, v) InterlockedExchange((long*)ptr, v)

#define __sync_lock_release(ptr) __sync_lock_test_and_set(ptr, 0)
#define  __sync_bool_compare_and_swap(ptr, o, n) InterlockedCompareExchange((long*)ptr, n, o)
#endif