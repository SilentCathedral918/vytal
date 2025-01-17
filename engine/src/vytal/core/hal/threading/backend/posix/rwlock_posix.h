#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API RWLock *hal_rwlock_construct(void);
VT_API Bool    hal_rwlock_destruct(RWLock *rwlock);
VT_API Bool    hal_rwlock_readlock(RWLock *rwlock);
VT_API Bool    hal_rwlock_writelock(RWLock *rwlock);
VT_API Bool    hal_rwlock_unlock(RWLock *rwlock);
