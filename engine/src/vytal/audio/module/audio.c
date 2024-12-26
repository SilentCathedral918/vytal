#include "audio.h"

#include "vytal/audio/backend/openal/audio_openal.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/misc/console/console.h"

#define ENGINE_AUDIO_BACKEND_DEFAULT (AUDIO_BACKEND_OPENAL)

typedef struct Audio_Module_State {
    AudioDevice   _device;
    AudioContext  _context;
    AudioListener _listener;
    AudioBackend  _backend;
} AudioModuleState;
static AudioModuleState *state = NULL;

ByteSize audio_module_get_size(void) { return sizeof(AudioModuleState); }

Bool audio_module_startup(VoidPtr module) {
    if (!module)
        return false;

    // assign module to state and init its members
    state = VT_CAST(AudioModuleState *, module);
    {
        state->_backend = ENGINE_AUDIO_BACKEND_DEFAULT;

        switch (state->_backend) {
        case AUDIO_BACKEND_OPENAL:
            state->_device  = audio_backend_al_open_device(NULL);
            state->_context = audio_backend_al_create_context(&state->_device, NULL);
            audio_backend_al_make_context_current(state->_context._handle);
            hal_mem_memzero(&state->_listener, sizeof(AudioListener));

            break;

        default:
            return false;
        }
    }

    return true;
}

Bool audio_module_shutdown(void) {
    if (!state)
        return false;

    // free and set members to zero
    {
        switch (state->_backend) {
        case AUDIO_BACKEND_OPENAL:
            audio_backend_al_make_context_current(NULL);

            if (!audio_backend_al_destroy_context(&state->_context))
                return false;

            if (!audio_backend_al_close_device(&state->_device))
                return false;

            break;

        default:
            return false;
        }
    }
    hal_mem_memzero(state, sizeof(AudioModuleState));

    state = NULL;
    return true;
}

Bool audio_module_update(void) { return true; }
