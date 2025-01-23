#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API RWLock *hal_rwlock_win32_construct(void);
VT_API Bool    hal_rwlock_win32_destruct(RWLock *rwlock);
VT_API Bool    hal_rwlock_win32_readlock(RWLock *rwlock);
VT_API Bool    hal_rwlock_win32_writelock(RWLock *rwlock);
VT_API Bool    hal_rwlock_win32_unlock_shared(RWLock *rwlock);
VT_API Bool    hal_rwlock_win32_unlock_exclusive(RWLock *rwlock);
