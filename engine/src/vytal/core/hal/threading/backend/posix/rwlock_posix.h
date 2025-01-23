#pragma once

#include "vytal/defines/core/thread.h"
#include "vytal/defines/shared.h"

VT_API RWLock *hal_rwlock_posix_construct(void);
VT_API Bool    hal_rwlock_posix_destruct(RWLock *rwlock);
VT_API Bool    hal_rwlock_posix_readlock(RWLock *rwlock);
VT_API Bool    hal_rwlock_posix_writelock(RWLock *rwlock);
VT_API Bool    hal_rwlock_posix_unlock(RWLock *rwlock);
