#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API Mutex   *hal_mutex_construct(void);
VT_API Bool     hal_mutex_destruct(Mutex *mutex);
VT_API Bool     hal_mutex_lock(Mutex *mutex);
VT_API Bool     hal_mutex_trylock(Mutex *mutex);
VT_API Bool     hal_mutex_unlock(Mutex *mutex);
VT_API CondVar *hal_condvar_construct(void);
VT_API Bool     hal_condvar_destruct(CondVar *cv);
VT_API Bool     hal_condvar_wait(CondVar *cv, Mutex *mutex);
VT_API Bool     hal_condvar_wait_timed(CondVar *cv, Mutex *mutex, const UInt32 timeout_ms);
VT_API Bool     hal_condvar_signal(CondVar *cv);
VT_API Bool     hal_condvar_broadcast(CondVar *cv);
VT_API RWLock  *hal_rwlock_construct(void);
VT_API Bool     hal_rwlock_destruct(RWLock *rwlock);
VT_API Bool     hal_rwlock_readlock(RWLock *rwlock);
VT_API Bool     hal_rwlock_writelock(RWLock *rwlock);
VT_API Bool     hal_rwlock_unlock(RWLock *rwlock, const Bool shared);
