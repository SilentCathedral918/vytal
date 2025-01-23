#include "condvar_win32.h"

#include "vytal/managers/memory/memmgr.h"

#include <windows.h>

typedef struct CondVar_Handle {
    PCONDITION_VARIABLE _handle;
} CondVarHandle;

typedef struct Mutex_Handle {
    LPCRITICAL_SECTION _handle;
} MutexHandle;

CondVar *hal_condvar_win32_construct(void) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    CondVar       *state_  = VT_CAST(CondVar *, chunk_);
    CondVarHandle *handle_ = VT_CAST(CondVarHandle *, state_ + 1);
    InitializeConditionVariable(handle_->_handle);

    state_->_handle = handle_;
    return state_;
}
Bool hal_condvar_win32_destruct(CondVar *cv) { return !cv ? false : memory_manager_deallocate(cv, MEMORY_TAG_THREADING); }
Bool hal_condvar_win32_wait(CondVar *cv, Mutex *mutex) { return (!cv || !mutex) ? false : SleepConditionVariableCS(VT_CAST(CondVarHandle *, cv->_handle)->_handle, VT_CAST(MutexHandle *, mutex->_handle)->_handle, INFINITE); }
Bool hal_condvar_win32_wait_timed(CondVar *cv, Mutex *mutex, const UInt32 timeout_ms) {
    return (!cv || !mutex) ? false : SleepConditionVariableCS(VT_CAST(CondVarHandle *, cv->_handle)->_handle, VT_CAST(MutexHandle *, mutex->_handle)->_handle, timeout_ms);
}
Bool hal_condvar_win32_signal(CondVar *cv) {
    if (!cv)
        return false;

    WakeConditionVariable(VT_CAST(CondVarHandle *, cv->_handle)->_handle);
    return true;
}
Bool hal_condvar_win32_broadcast(CondVar *cv) {
    if (!cv)
        return false;

    WakeAllConditionVariable(VT_CAST(CondVarHandle *, cv->_handle)->_handle);
    return true;
}
