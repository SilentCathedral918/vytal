#include "audio.h"

#include "vytal/audio/backend/openal/audio_openal.h"
#include "vytal/audio/core/loaders/audio_loader.h"
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

Bool _audio_module_play_audio_openal(ConstStr filepath) {
    AudioData data_ = audio_core_load_from_file(filepath);

    UInt32 buffer_ = 0;
    if (!audio_backend_al_generate_buffer(&buffer_)) {
        misc_console_writeln("failed to generate openal buffer");

        audio_backend_al_destroy_context(&state->_context);
        audio_backend_al_close_device(&state->_device);
        audio_core_unload_data(&data_);
        return false;
    }

    if (!audio_backend_al_buffer_fill_data(buffer_, AUDIO_CHANNEL_MONO_8, data_._pcm_data, data_._data_size,
                                           data_._sample_rate)) {
        misc_console_writeln("failed to fill data to openal buffer");

        audio_backend_al_destroy_context(&state->_context);
        audio_backend_al_close_device(&state->_device);
        audio_core_unload_data(&data_);
        return false;
    }

    UInt32 source_ = 0;
    if (!audio_backend_al_generate_source(&source_)) {
        misc_console_writeln("failed to generate openal source");

        audio_backend_al_destroy_context(&state->_context);
        audio_backend_al_close_device(&state->_device);
        audio_core_unload_data(&data_);
        return false;
    }

    // attach buffer to the source
    audio_backend_al_attach_buffer_to_source(source_, buffer_);

    // play the source
    audio_backend_al_source_play(source_);

    // until the source finishes playing
    AudioPlaybackState state_;
    do {
        state_ = audio_backend_al_get_playback_state(source_);
    } while (state_ == AUDIO_PLAYBACK_PLAYING);

    misc_console_writeln("playback finished.");

    // clean-up
    audio_backend_al_delete_source(&source_);
    audio_backend_al_delete_buffer(&buffer_);
    audio_core_unload_data(&data_);

    return true;
}

ByteSize audio_module_get_size(void) { return sizeof(AudioModuleState); }

Bool audio_module_startup(VoidPtr module) {
    if (!module)
        return false;

    // assign module to state and init its members
    state = VT_CAST(AudioModuleState *, module);
    {
        state->_backend = ENGINE_AUDIO_BACKEND_DEFAULT; // TODO: configure this with cvar

        switch (state->_backend) {
        case AUDIO_BACKEND_OPENAL:
            // device
            state->_device = audio_backend_al_open_device(NULL);

            // context
            state->_context = audio_backend_al_create_context(&state->_device, NULL);
            audio_backend_al_make_context_current(state->_context._handle);

            // listener
            audio_backend_al_set_listener_position(&state->_listener, 0.f, 0.f, 0.f);
            audio_backend_al_set_listener_velocity(&state->_listener, 0.f, 0.f, 0.f);
            const Flt32 orientation_[6] = {0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};
            audio_backend_al_set_listener_orientation(&state->_listener, VT_CAST(Flt32 *, orientation_));

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
            // context
            audio_backend_al_make_context_current(NULL);
            if (!audio_backend_al_destroy_context(&state->_context))
                return false;

            // device
            if (!audio_backend_al_close_device(&state->_device))
                return false;

            break;

        default:
            return false;
        }

        hal_mem_memzero(state, sizeof(AudioModuleState));
    }

    state = NULL;
    return true;
}

Bool audio_module_update(void) { return true; }

Bool audio_module_play_audio(ConstStr filepath) {
    if (!filepath)
        return false;

    switch (state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        return _audio_module_play_audio_openal(filepath);

    default:
        return false;
    }
}
