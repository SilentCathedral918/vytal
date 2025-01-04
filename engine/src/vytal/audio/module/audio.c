#include "audio.h"

#include "vytal/audio/backend/openal/audio_openal.h"
#include "vytal/audio/core/loaders/audio_loader.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/memory/allocators/pool.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/platform/filesystem/filesystem.h"

#define ENGINE_AUDIO_BACKEND_DEFAULT (AUDIO_BACKEND_OPENAL)
#define ENGINE_AUDIO_ALLOCATOR_CAPACITY (VT_SIZE_MB_MULT(16)) // 16 MB

typedef struct Audio_Module_State {
    AudioDevice   _device;
    AudioContext  _context;
    AudioListener _listener;
    AudioBackend  _backend;

    PoolAllocator _buffer_allocator;
    Map           _buffer_map;
    PoolAllocator _source_allocator;
    Map           _source_map;
    PoolAllocator _data_allocator;
    Map           _data_map;
} AudioModuleState;
static AudioModuleState *state = NULL;

UInt32 _audio_module_al_generate_buffer(ConstStr audio_id, const AudioData *data) {
    if (!data)
        return -1;

    UInt32 buffer_ = 0;
    if (!audio_backend_al_generate_buffer(&buffer_))
        return -1;

    if (!audio_backend_al_buffer_fill_data(buffer_, data->_channel_format, data->_pcm_data, data->_data_size,
                                           data->_sample_rate))
        return -1;

    return buffer_;
}

UInt32 _audio_module_al_generate_source(UInt32 buffer_id, const Flt32 position[3], const Flt32 velocity[3],
                                        const Flt32 direction[3], const Bool omnidirectional, const Flt32 pitch,
                                        const Flt32 volume, const Bool loop) {
    UInt32 source_ = 0;
    if (!audio_backend_al_generate_source(&source_))
        return -1;

    // attach buffer to the source
    audio_backend_al_attach_buffer_to_source(source_, buffer_id);

    // set source position, velocity and direction
    {
        audio_backend_al_set_source_position(source_, position[0], position[1], position[2]);
        audio_backend_al_set_source_velocity(source_, velocity[0], velocity[1], velocity[2]);

        if (!audio_backend_al_set_source_direction(source_, direction, omnidirectional))
            return -1;
    }

    // set looping state
    audio_backend_al_set_looping(source_, loop);

    return source_;
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

            // allocators
            state->_buffer_allocator = allocator_pool_construct(ENGINE_AUDIO_ALLOCATOR_CAPACITY, 1);
            state->_source_allocator = allocator_pool_construct(ENGINE_AUDIO_ALLOCATOR_CAPACITY, 1);
            state->_data_allocator   = allocator_pool_construct(ENGINE_AUDIO_ALLOCATOR_CAPACITY, 1);

            // allocate state map members
            {
                // buffers
                state->_buffer_map = container_map_construct(sizeof(AudioBuffer), state->_buffer_allocator);

                // sources
                state->_source_map = container_map_construct(sizeof(AudioSource), state->_source_allocator);

                // audio datas
                state->_data_map = container_map_construct(sizeof(AudioData), state->_data_allocator);
            }

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
            // deallocate state map members
            {
                // buffers
                if (!container_map_destruct(state->_buffer_map))
                    return false;

                // sources
                if (!container_map_destruct(state->_source_map))
                    return false;

                // audio datas
                if (!container_map_destruct(state->_data_map))
                    return false;
            }

            // allocators
            {
                if (!allocator_pool_destruct(state->_buffer_allocator))
                    return false;

                if (!allocator_pool_destruct(state->_source_allocator))
                    return false;

                if (!allocator_pool_destruct(state->_data_allocator))
                    return false;
            }

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

AudioData *audio_module_load_audio(ConstStr id, ConstStr filepath) {
    if (!state || !id || !filepath)
        return NULL;

    AudioData data_ = audio_core_load_from_file(filepath, state->_data_map);

    if (!container_map_insert(state->_data_map, id, &data_))
        return NULL;

    return container_map_get(state->_data_map, AudioData, id);
}

Bool audio_module_unload_audio(ConstStr id) {
    if (!state || !id)
        return false;

    if (!audio_core_unload_data(id, state->_data_map))
        return false;

    return container_map_remove(state->_data_map, id);
}

AudioData *audio_module_get_audio(ConstStr id) { return container_map_get(state->_data_map, AudioData, id); }

AudioBuffer *audio_module_construct_buffer(ConstStr buffer_id, ConstStr audio_id) {
    if (!state || !buffer_id || !audio_id)
        return NULL;

    AudioData *data_ = container_map_get(state->_data_map, AudioData, audio_id);
    if (!data_)
        return NULL;

    UInt32 buffer_id_ = -1;
    {
        switch (state->_backend) {
        case AUDIO_BACKEND_OPENAL:
            buffer_id_ = _audio_module_al_generate_buffer(audio_id, data_);
            if (buffer_id_ == -1)
                return NULL;

            break;

        default:
            return NULL;
        }
    }

    AudioBuffer buffer_ = VT_STRUCT(AudioBuffer, ._id = buffer_id_, ._data = data_, ._metadata = NULL);

    if (!container_map_insert(state->_buffer_map, buffer_id, &buffer_))
        return NULL;

    ++data_->_num_refs;
    return container_map_get(state->_buffer_map, AudioBuffer, buffer_id);
}

AudioSource *audio_module_construct_source(ConstStr id, const Bool loop) {
    if (!state || !id)
        return NULL;

    Flt32 position_[3]     = {0.0f, 0.0f, 0.0f};
    Flt32 velocity_[3]     = {0.0f, 0.0f, 0.0f};
    Flt32 direction_[3]    = {0.0f, 0.0f, 0.0f};
    Flt32 pitch_           = 1.0f;
    Flt32 volume_          = 1.0f;
    Bool  omnidirectional_ = true;

    UInt32 source_id_ = -1;
    {
        switch (state->_backend) {
        case AUDIO_BACKEND_OPENAL:
            source_id_ =
                _audio_module_al_generate_source(-1, position_, velocity_, direction_, omnidirectional_, pitch_, volume_, loop);
            if (source_id_ == -1)
                return NULL;

            break;

        default:
            return NULL;
        }
    }

    AudioSource source_ = {._id                = source_id_,
                           ._loop              = loop,
                           ._metadata          = NULL,
                           ._omnidirectional   = omnidirectional_,
                           ._pitch             = pitch_,
                           ._playback_state    = AUDIO_PLAYBACK_STOPPED,
                           ._playback_position = 0.0f,
                           ._volume            = volume_};
    hal_mem_memcpy(&source_._position, position_, sizeof(source_._position));
    hal_mem_memcpy(&source_._velocity, velocity_, sizeof(source_._velocity));
    hal_mem_memcpy(&source_._direction, direction_, sizeof(source_._direction));

    if (!container_map_insert(state->_source_map, id, &source_))
        return NULL;

    return container_map_get(state->_source_map, AudioSource, id);
}

AudioSource *audio_module_construct_source_with_buffer(ConstStr source_id, ConstStr buffer_id, const Bool loop) {
    if (!state || !source_id || !buffer_id)
        return NULL;

    Flt32 position_[3]     = {0.0f, 0.0f, 0.0f};
    Flt32 velocity_[3]     = {0.0f, 0.0f, 0.0f};
    Flt32 direction_[3]    = {0.0f, 0.0f, 0.0f};
    Flt32 pitch_           = 1.0f;
    Flt32 volume_          = 1.0f;
    Bool  omnidirectional_ = true;

    AudioBuffer *buffer_ = container_map_get(state->_buffer_map, AudioBuffer, buffer_id);
    if (!buffer_)
        return NULL;

    UInt32 source_id_ = -1;
    {
        switch (state->_backend) {
        case AUDIO_BACKEND_OPENAL:
            source_id_ = _audio_module_al_generate_source(buffer_->_id, position_, velocity_, direction_, omnidirectional_,
                                                          pitch_, volume_, loop);
            if (source_id_ == -1)
                return NULL;

            break;

        default:
            return NULL;
        }
    }

    AudioSource source_ = {._id              = source_id_,
                           ._loop            = loop,
                           ._metadata        = NULL,
                           ._omnidirectional = omnidirectional_,
                           ._pitch           = pitch_,
                           ._playback_state  = AUDIO_PLAYBACK_STOPPED,
                           ._volume          = volume_};
    hal_mem_memcpy(&source_._position, position_, sizeof(source_._position));
    hal_mem_memcpy(&source_._velocity, velocity_, sizeof(source_._velocity));
    hal_mem_memcpy(&source_._direction, direction_, sizeof(source_._direction));

    if (!container_map_insert(state->_source_map, source_id, &source_))
        return NULL;

    return container_map_get(state->_source_map, AudioSource, source_id);
}

Bool audio_module_destruct_buffer(ConstStr id) {
    if (!state || !id)
        return false;

    AudioBuffer *buffer_ = container_map_get(state->_buffer_map, AudioBuffer, id);
    if (!buffer_)
        return false;

    AudioData *data_ = buffer_->_data;
    if (data_)
        --data_->_num_refs;

    {
        switch (state->_backend) {
        case AUDIO_BACKEND_OPENAL:
            if (!audio_backend_al_delete_buffer(&buffer_->_id))
                return false;
            break;

        default:
            return false;
        }
    }

    if (!container_map_remove(state->_buffer_map, id))
        return false;

    buffer_ = NULL;
    return true;
}

Bool audio_module_destruct_source(ConstStr id) {
    if (!state || !id)
        return false;

    AudioSource *source_ = container_map_get(state->_source_map, AudioSource, id);
    if (!source_)
        return false;

    {
        switch (state->_backend) {
        case AUDIO_BACKEND_OPENAL:
            if (!audio_backend_al_delete_source(&source_->_id))
                return false;
            break;

        default:
            return false;
        }
    }

    if (!container_map_remove(state->_source_map, id))
        return false;

    source_ = NULL;
    return true;
}

AudioBuffer *audio_module_get_buffer(ConstStr id) { return container_map_get(state->_buffer_map, AudioBuffer, id); }

AudioSource *audio_module_get_source(ConstStr id) { return container_map_get(state->_source_map, AudioSource, id); }

Bool audio_module_assign_buffer_to_source(ConstStr source_id, ConstStr buffer_id) {
    if (!state || !source_id || !buffer_id)
        return false;

    AudioSource *source_ = container_map_get(state->_source_map, AudioSource, source_id);
    if (!source_)
        return false;

    AudioBuffer *buffer_ = container_map_get(state->_buffer_map, AudioBuffer, buffer_id);
    if (!buffer_)
        return false;

    switch (state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_attach_buffer_to_source(source_->_id, buffer_->_id);
        return true;

    default:
        return false;
    }
}

VT_API VoidPtr audio_module_get_state(void) { return state; }
