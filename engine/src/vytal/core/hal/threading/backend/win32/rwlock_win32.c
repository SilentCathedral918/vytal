#include "rwlock_win32.h"

#include "vytal/managers/memory/memmgr.h"

#include <windows.h>

RWLock *hal_rwlock_win32_construct(void) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    RWLock  *state_  = VT_CAST(RWLock *, chunk_);
    PSRWLOCK handle_ = VT_CAST(PSRWLOCK, state_ + 1);
    InitializeSRWLock(handle_);

    state_->_handle = VT_CAST(VoidPtr, handle_);
    return state_;
}
Bool hal_rwlock_win32_destruct(RWLock *rwlock) { return (!rwlock) ? false : memory_manager_deallocate(rwlock, MEMORY_TAG_THREADING); }
Bool hal_rwlock_win32_readlock(RWLock *rwlock) {
    if (!rwlock)
        return false;

    AcquireSRWLockShared(rwlock->_handle);
    return true;
}
Bool hal_rwlock_win32_writelock(RWLock *rwlock) {
    if (!rwlock)
        return false;

    AcquireSRWLockExclusive(rwlock->_handle);
    return true;
}
Bool hal_rwlock_win32_unlock_shared(RWLock *rwlock) {
    if (!rwlock)
        return false;

    ReleaseSRWLockShared(rwlock->_handle);
    return true;
}
Bool hal_rwlock_win32_unlock_exclusive(RWLock *rwlock) {
    if (!rwlock)
        return false;

    ReleaseSRWLockExclusive(rwlock->_handle);
    return true;
}
