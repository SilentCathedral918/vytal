#include "thread.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/hal/threading/core/thread/thread.h"

#define MAX_POOL_THREADS 4

typedef struct Thread_Pool {
    Thread *_threads[MAX_POOL_THREADS];
    Bool    _available[MAX_POOL_THREADS];
    UInt32  _size;
} ThreadPool;

typedef struct Thread_Module_State {
    ThreadBackend _backend;
    ThreadPool    _pool;
} ThreadModuleState;
static ThreadModuleState *state = NULL;

Bool _thread_pool_startup(void) {
    if (!state)
        return false;

    for (UInt32 i = 0; i < MAX_POOL_THREADS; ++i) {
        state->_pool._threads[i]   = hal_thread_construct(NULL, NULL);
        state->_pool._available[i] = true;
    }

    state->_pool._size = MAX_POOL_THREADS;
    return true;
}
Bool _thread_pool_shutdown(void) {
    if (!state)
        return false;

    for (UInt32 i = 0; i < state->_pool._size; ++i) {
        Thread *thread_ = state->_pool._threads[i];
        if (thread_) {
            // finish all the busy threads
            if (!state->_pool._available[i])
                hal_thread_join(thread_);

            hal_thread_destruct(thread_);

            state->_pool._threads[i]   = NULL;
            state->_pool._available[i] = true;
        }
    }

    state->_pool._size = 0;
    return true;
}
Bool _thread_module_wait_for_all(void) {
    if (!state)
        return false;

    for (UInt32 i = 0; i < state->_pool._size; ++i) {
        Thread *thread_ = state->_pool._threads[i];
        if (thread_) {
            // finish all the busy threads
            if (!state->_pool._available[i])
                hal_thread_join(thread_);

            hal_thread_destruct(thread_);

            state->_pool._threads[i]   = NULL;
            state->_pool._available[i] = true;
        }
    }

    return true;
}

ByteSize thread_module_get_size(void) { return sizeof(ThreadModuleState); }

Bool thread_module_startup(VoidPtr module, const ThreadBackend backend) {
    if (!module)
        return false;

    state           = VT_CAST(ThreadModuleState *, module);
    state->_backend = backend;

    return _thread_pool_startup();
}
Bool thread_module_shutdown(void) {
    if (!state)
        return false;

    if (!_thread_module_wait_for_all())
        return false;

    if (!_thread_pool_shutdown())
        return false;

    hal_mem_memzero(state, sizeof(ThreadModuleState));
    state = NULL;
    return true;
}
Thread *thread_module_request_thread(void) {
    for (UInt32 i = 0; i < state->_pool._size; ++i) {
        if (state->_pool._available[i]) {
            state->_pool._available[i] = false;
            return state->_pool._threads[i];
        }
    }

    return NULL;
}
Bool thread_module_release_thread(Thread *thread) {
    if (!thread)
        return false;

    for (UInt32 i = 0; i < state->_pool._size; ++i) {
        if (state->_pool._threads[i] == thread) {
            state->_pool._available[i] = true;
            return true;
        }
    }

    return false;
}
VoidPtr       thread_module_get_state(void) { return state; }
ThreadBackend thread_module_get_backend(void) { return state->_backend; }
ByteSize      thread_module_get_thread_count(void) { return state->_pool._size; }
void          thread_module_switch_backend(const ThreadBackend backend) {
    if (state->_backend == backend)
        return;

    // make sure that all active threads are cleaned up before switching
    if (!_thread_module_wait_for_all())
        return;

    state->_backend = backend;
}
