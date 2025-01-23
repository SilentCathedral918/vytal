#include "thread.h"

#include "vytal/core/hal/threading/backend/posix/thread_posix.h"
#include "vytal/core/hal/threading/backend/win32/thread_win32.h"
#include "vytal/core/hal/threading/module/thread.h"

Thread *hal_thread_construct(VoidPtr func, VoidPtr args) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_thread_posix_construct(func, args);

    case THREAD_BACKEND_WIN32:
        return hal_thread_win32_construct(func, args);

    default:
        return NULL;
    }
}
Bool hal_thread_join(Thread *thread) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_thread_posix_join(thread);

    case THREAD_BACKEND_WIN32:
        return hal_thread_win32_join(thread);

    default:
        return false;
    }
}
Bool hal_thread_destruct(Thread *thread) {
    switch (thread_module_get_backend()) {
    case THREAD_BACKEND_POSIX:
        return hal_thread_posix_destruct(thread);

    case THREAD_BACKEND_WIN32:
        return hal_thread_win32_destruct(thread);

    default:
        return false;
    }
}
