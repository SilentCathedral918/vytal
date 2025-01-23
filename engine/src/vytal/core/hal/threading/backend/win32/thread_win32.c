#include "thread_win32.h"

#include "vytal/managers/memory/memmgr.h"

#include <windows.h>

Thread *hal_thread_win32_construct(VoidPtr func, VoidPtr args) {
    VoidPtr chunk_ = memory_manager_allocate(0, MEMORY_TAG_THREADING);
    if (!chunk_)
        return NULL;

    Thread *state_    = VT_CAST(Thread *, chunk_);
    state_->_state    = THREAD_STATE_CREATED;
    state_->_priority = 0;
    state_->_handle   = CreateThread(NULL, 0, VT_CAST(LPTHREAD_START_ROUTINE, func), args, 0, NULL);

    return state_;
}
Bool hal_thread_win32_join(Thread *thread) {
    if (!thread)
        return false;

    if (WaitForSingleObject(thread->_handle, INFINITE) == 0xFFFFFFFF)
        return false;

    CloseHandle(thread->_handle);
    thread->_state = THREAD_STATE_TERMINATED;
    return true;
}
Bool hal_thread_win32_destruct(Thread *thread) { return (!thread || (thread->_state != THREAD_STATE_TERMINATED)) ? false : (CloseHandle(thread->_handle) != 0) && memory_manager_deallocate(thread, MEMORY_TAG_THREADING); }
