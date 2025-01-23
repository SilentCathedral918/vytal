#include "thread.h"

#include "vytal/core/hal/memory/vtmem.h"

typedef struct Thread_Module_State {
    ThreadBackend _backend;
} ThreadModuleState;
static ThreadModuleState *state = NULL;

ByteSize thread_module_get_size(void) { return sizeof(ThreadModuleState); }

Bool thread_module_startup(VoidPtr module, const ThreadBackend backend) {
    if (!module)
        return false;

    state           = VT_CAST(ThreadModuleState *, module);
    state->_backend = backend;
    return true;
}
Bool thread_module_shutdown(void) {
    if (!state)
        return false;

    hal_mem_memzero(state, sizeof(ThreadModuleState));
    state = NULL;
    return true;
}
VoidPtr       thread_module_get_state(void) { return state; }
ThreadBackend thread_module_get_backend(void) { return state->_backend; }
