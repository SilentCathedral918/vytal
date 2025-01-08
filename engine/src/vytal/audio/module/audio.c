#include "audio.h"

#include "vytal/audio/backend/openal/audio_openal.h"
#include "vytal/audio/core/loaders/audio_loader.h"
#include "vytal/core/containers/array/array.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/hash/hash.h"
#include "vytal/core/memory/allocators/arena.h"
#include "vytal/core/memory/allocators/pool.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/platform/filesystem/filesystem.h"

#define ENGINE_AUDIO_BACKEND_DEFAULT (AUDIO_BACKEND_OPENAL)
#define ENGINE_AUDIO_ALLOCATOR_CAPACITY (VT_SIZE_MB_MULT(16))         // 16 MB
#define ENGINE_AUDIO_TRACKER_CAPACITY (VT_SIZE_MB_MULT(4))            // 4 MB
#define ENGINE_AUDIO_TRANSITION_TASKS_CAPACITY (VT_SIZE_MB_MULT(4))   // 4 MB
#define ENGINE_AUDIO_SEQUENCES_CAPACITY (VT_SIZE_MB_MULT(2))          // 2 MB
#define ENGINE_AUDIO_SEQUENCE_TASK_POOL_CAPACITY (VT_SIZE_MB_MULT(4)) // 4 MB

typedef struct Audio_Module_State {
    AudioDevice    _device;
    AudioContext   _context;
    AudioListener  _listener;
    AudioBackend   _backend;
    PoolAllocator  _sequence_task_pool;
    ArenaAllocator _allocator;
    Map            _buffer_map;
    Map            _source_map;
    Map            _audio_map;
    Array          _loaded_audios;
    Array          _loaded_buffers;
    Array          _active_sources;
    Array          _transition_tasks;
    Array          _sequences;
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

    // set source volume and pitch
    {
        audio_backend_al_set_source_volume(source_, volume);
        audio_backend_al_set_source_pitch(source_, pitch);
    }

    // set looping state
    audio_backend_al_set_source_looping(source_, loop);

    return source_;
}

AudioData *_audio_module_search_loaded_audio(const UInt64 id) {
    for (ByteSize i = 0; i < container_array_length(state->_loaded_audios); ++i) {
        AudioData *data_ = container_array_get_at_index(state->_loaded_audios, i);
        if (data_ && data_->_id == id)
            return data_;
    }

    return NULL;
}

AudioBuffer *_audio_module_search_loaded_buffer(const UInt32 id) {
    for (ByteSize i = 0; i < container_array_length(state->_loaded_buffers); ++i) {
        AudioBuffer *buffer_ = container_array_get_at_index(state->_loaded_buffers, i);
        if (buffer_ && buffer_->_id == id)
            return buffer_;
    }

    return NULL;
}

AudioSource *_audio_module_search_active_source(const UInt32 id) {
    for (ByteSize i = 0; i < container_array_length(state->_active_sources); ++i) {
        AudioSource *source_ = container_array_get_at_index(state->_active_sources, i);
        if (source_ && source_->_id == id)
            return source_;
    }

    return NULL;
}

Bool _audio_module_trackers_cleanup(void) {
    switch (state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        // loaded audio sources
        for (ByteSize i = 0; i < container_array_length(state->_active_sources); ++i) {
            AudioSource *source_ = container_array_get_at_index(state->_active_sources, i);
            if (!audio_backend_al_delete_source(&source_->_id))
                return false;
        }

        // loaded audio buffers
        for (ByteSize i = 0; i < container_array_length(state->_loaded_buffers); ++i) {
            AudioBuffer *buffer_ = container_array_get_at_index(state->_loaded_buffers, i);
            if (!audio_backend_al_delete_buffer(&buffer_->_id))
                return false;
        }

        // loaded audio data
        for (ByteSize i = 0; i < container_array_length(state->_loaded_audios); ++i) {
            AudioData *data_ = container_array_get_at_index(state->_loaded_audios, i);
            if (!audio_core_unload_data(data_))
                return false;
        }

        return true;

    default:
        return false;
    }
}

ByteSize audio_module_get_size(void) { return sizeof(AudioModuleState); }

Bool audio_module_startup(VoidPtr module) {
    if (!module)
        return false;

    // assign module to state and init its members
    state = VT_CAST(AudioModuleState *, module);
    {
        state->_backend = ENGINE_AUDIO_BACKEND_DEFAULT; // TODO: configure this

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

    // sequence task pool
    state->_sequence_task_pool = allocator_pool_construct(ENGINE_AUDIO_SEQUENCE_TASK_POOL_CAPACITY, 256);

    // allocator
    state->_allocator = allocator_arena_construct((ENGINE_AUDIO_ALLOCATOR_CAPACITY * 3) + (ENGINE_AUDIO_TRACKER_CAPACITY * 3) +
                                                  ENGINE_AUDIO_TRANSITION_TASKS_CAPACITY + ENGINE_AUDIO_SEQUENCES_CAPACITY);

    // allocate state map members
    {
        // buffers
        state->_buffer_map =
            container_map_construct_custom(sizeof(AudioBuffer), state->_allocator, ENGINE_AUDIO_ALLOCATOR_CAPACITY);

        // sources
        state->_source_map =
            container_map_construct_custom(sizeof(AudioSource), state->_allocator, ENGINE_AUDIO_ALLOCATOR_CAPACITY);

        // audio datas
        state->_audio_map =
            container_map_construct_custom(sizeof(AudioData), state->_allocator, ENGINE_AUDIO_ALLOCATOR_CAPACITY);
    }

    // allocate state tracker members
    {
        // loaded audio data
        state->_loaded_audios =
            container_array_construct_custom(AudioData, state->_allocator, ALLOCTYPE_ARENA, ENGINE_AUDIO_TRACKER_CAPACITY);

        // loaded audio buffers
        state->_loaded_buffers =
            container_array_construct_custom(AudioBuffer, state->_allocator, ALLOCTYPE_ARENA, ENGINE_AUDIO_TRACKER_CAPACITY);

        // active audio sources
        state->_active_sources =
            container_array_construct_custom(AudioSource, state->_allocator, ALLOCTYPE_ARENA, ENGINE_AUDIO_TRACKER_CAPACITY);
    }

    // allocate transition tasks member
    state->_transition_tasks = container_array_construct_custom(AudioTransitionTask, state->_allocator, ALLOCTYPE_ARENA,
                                                                ENGINE_AUDIO_TRANSITION_TASKS_CAPACITY);

    // allocate sequences member
    state->_sequences =
        container_array_construct_custom(AudioSequence, state->_allocator, ALLOCTYPE_ARENA, ENGINE_AUDIO_SEQUENCES_CAPACITY);

    return true;
}

Bool audio_module_shutdown(void) {
    if (!state)
        return false;

    // free and set members to zero
    {
        // deallocate sequences member
        if (!container_array_destruct(state->_sequences))
            return false;

        // deallocate transition tasks member
        if (!container_array_destruct(state->_transition_tasks))
            return false;

        // deallocate state tracker members
        {
            if (!_audio_module_trackers_cleanup())
                return false;

            // loaded audio data
            if (!container_array_destruct(state->_loaded_audios))
                return false;

            // loaded audio buffers
            if (!container_array_destruct(state->_loaded_buffers))
                return false;

            // active audio sources
            if (!container_array_destruct(state->_active_sources))
                return false;
        }

        // deallocate state map members
        {
            // buffers
            if (!container_map_destruct(state->_buffer_map))
                return false;

            // sources
            if (!container_map_destruct(state->_source_map))
                return false;

            // audio datas
            if (!container_map_destruct(state->_audio_map))
                return false;
        }

        // allocator
        allocator_arena_destruct(state->_allocator);

        // sequence task pool
        allocator_pool_destruct(state->_sequence_task_pool);

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

Bool audio_module_update(const Flt32 delta_time, const Flt32 fixed_update_time) {
    // audio transition tasks
    for (ByteSize i = 0; i < container_array_length(state->_transition_tasks); ++i) {
        AudioTransitionTask *task_ = container_array_get_at_index(state->_transition_tasks, i);
        if (!task_)
            continue;

        task_->_elapsed_ms += 1000.0f * fixed_update_time;
        if (task_->_elapsed_ms > task_->_duration_ms)
            task_->_elapsed_ms = task_->_duration_ms;

        // calculate progress and clamp it between 0 and 1
        Flt32 progress_ = VT_CAST(Flt32, task_->_elapsed_ms) / task_->_duration_ms;
        progress_       = (progress_ > 1.0f) ? 1.0f : progress_; // Clamp progress to 1.0f

        // apply transition with the clamped progress
        task_->_apply_transition(task_->_source, task_->_current_data, task_->_target_data, progress_);

        // when done...
        if (task_->_elapsed_ms >= task_->_duration_ms) {
            // ensure current value is reached exactly to target value
            task_->_apply_transition(task_->_source, task_->_current_data, task_->_target_data, 1.0f);

            // task done, remove
            container_array_remove_at_index(state->_transition_tasks, i);

            --i; // prevent skipping the next task
        }
    }

    // audio sequences
    for (ByteSize i = 0; i < container_array_length(state->_sequences); ++i) {
        AudioSequence *sequence_ = container_array_get_at_index(state->_sequences, i);
        if (!sequence_)
            continue;

        if (container_array_isempty(sequence_->_tasks))
            continue;

        sequence_->_elapsed_ms += 1000.0f * fixed_update_time;

        Flt32 sequence_progress_ = VT_CAST(Flt32, sequence_->_elapsed_ms) / sequence_->_duration_ms;
        sequence_progress_       = (sequence_progress_ > 1.0f) ? 1.0f : sequence_progress_;

        for (ByteSize j = 0; j < container_array_length(sequence_->_tasks); ++j) {
            AudioSequenceTask *task_ = container_array_get_at_index(sequence_->_tasks, j);
            if (!task_)
                continue;

            if (sequence_->_elapsed_ms < task_->_start_time_ms)
                continue;

            Flt32 task_progress_ = VT_CAST(Flt32, sequence_->_elapsed_ms - task_->_start_time_ms) / task_->_duration_ms;
            task_progress_       = (task_progress_ > 1.0f) ? 1.0f : task_progress_;

            task_->_apply_transition(task_->_source, task_->_source_other, task_->_current_data, task_->_target_data,
                                     task_progress_);

            const UInt32 task_end_time_ = task_->_start_time_ms + task_->_duration_ms;
            if (sequence_->_elapsed_ms >= task_end_time_) {
                container_array_remove_at_index(sequence_->_tasks, j);
                --j;
            }
        }

        if (!sequence_->_retain && sequence_->_elapsed_ms >= sequence_->_duration_ms) {
            container_array_remove_at_index(state->_sequences, i);
            --i;
        }
    }

    return true;
}

AudioData *audio_module_load_audio(ConstStr id, ConstStr filepath) {
    if (!state || !id || !filepath)
        return NULL;

    AudioData data_ = audio_core_load_from_file(filepath, state->_audio_map);
    data_._id       = hash_hashstr(id, HASH_MODE_XX64);

    if (!container_map_insert(state->_audio_map, id, &data_))
        return NULL;

    container_array_push(state->_loaded_audios, AudioData, data_);

    return container_map_get(state->_audio_map, AudioData, id);
}

Bool audio_module_unload_audio(ConstStr id) {
    if (!state || !id)
        return false;

    if (!audio_core_unload_data_by_id(id, state->_audio_map))
        return false;

    AudioData *data_ = _audio_module_search_loaded_audio(hash_hashstr(id, HASH_MODE_XX64));
    if (!data_)
        return false;

    container_array_remove(state->_loaded_audios, data_);

    return container_map_remove(state->_audio_map, id);
}

AudioData *audio_module_get_audio(ConstStr id) { return container_map_get(state->_audio_map, AudioData, id); }

AudioBuffer *audio_module_construct_buffer(ConstStr buffer_id, ConstStr audio_id) {
    if (!state || !buffer_id || !audio_id)
        return NULL;

    AudioData *data_ = container_map_get(state->_audio_map, AudioData, audio_id);
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
    Flt32 direction_[3]    = {0.0f, 0.0f, -1.0f};
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

    container_array_push(state->_active_sources, AudioSource, source_);

    return container_map_get(state->_source_map, AudioSource, id);
}

AudioSource *audio_module_construct_source_with_buffer(ConstStr source_id, ConstStr buffer_id, const Bool loop) {
    if (!state || !source_id || !buffer_id)
        return NULL;

    Flt32 position_[3]     = {0.0f, 0.0f, 0.0f};
    Flt32 velocity_[3]     = {0.0f, 0.0f, 0.0f};
    Flt32 direction_[3]    = {0.0f, 0.0f, -1.0f};
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

    container_array_push(state->_active_sources, AudioSource, source_);

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

AudioBuffer *audio_module_get_loaded_buffer(ConstStr id) {
    if (!id)
        return NULL;

    AudioBuffer *buffer_ = container_map_get(state->_buffer_map, AudioBuffer, id);
    if (!buffer_)
        return NULL;

    return _audio_module_search_loaded_buffer(buffer_->_id);
}

AudioSource *audio_module_get_active_source(ConstStr id) {
    if (!id)
        return NULL;

    AudioSource *source_ = container_map_get(state->_source_map, AudioSource, id);
    if (!source_)
        return NULL;

    return _audio_module_search_active_source(source_->_id);
}

VoidPtr audio_module_get_state(void) { return state; }

VoidPtr audio_module_get_transition_tasks(void) { return state->_transition_tasks; }

VoidPtr audio_module_get_sequence_task_pool(void) { return state->_sequence_task_pool; }

VoidPtr audio_module_get_sequences(void) { return state->_sequences; }
