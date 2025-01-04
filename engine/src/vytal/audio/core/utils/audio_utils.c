#include "audio_utils.h"

#include "vytal/audio/backend/openal/audio_openal.h"
#include "vytal/audio/module/audio.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/hal/clock/hiresclock.h"
#include "vytal/core/hal/memory/vtmem.h"

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

#define audio_module_state (VT_CAST(AudioModuleState *, audio_module_get_state()))

// ---------------------------------- source ---------------------------------- //

Bool audio_utils_source_set_position(AudioSource *source, Flt32 x, Flt32 y, Flt32 z) {
    if (!source)
        return false;

    source->_position[0] = x;
    source->_position[1] = y;
    source->_position[2] = z;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_position(source->_id, source->_position[0], source->_position[1], source->_position[2]);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_translate(AudioSource *source, Flt32 dx, Flt32 dy, Flt32 dz) {
    if (!source)
        return false;

    source->_position[0] += dx;
    source->_position[1] += dy;
    source->_position[2] += dz;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_position(source->_id, source->_position[0], source->_position[1], source->_position[2]);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_set_direction(AudioSource *source, Flt32 x, Flt32 y, Flt32 z) {
    if (!source)
        return false;

    source->_direction[0] = x;
    source->_direction[1] = y;
    source->_direction[2] = z;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_direction(source->_id, source->_direction, source->_omnidirectional);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_rotate(AudioSource *source, Flt32 dx, Flt32 dy, Flt32 dz) {
    if (!source)
        return false;

    source->_direction[0] += dx;
    source->_direction[1] += dy;
    source->_direction[2] += dz;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_direction(source->_id, source->_direction, source->_omnidirectional);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_reset_direction(AudioSource *source) {
    if (!source)
        return false;

    Flt32 default_direction_[3] = {0.0f, 0.0f, 0.0f};
    hal_mem_memcpy(&source->_direction, &default_direction_, sizeof(source->_direction));

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_direction(source->_id, default_direction_, source->_omnidirectional);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_play(AudioSource *source) {
    if (!source)
        return false;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_source_play(source->_id);
        source->_playback_state = audio_backend_al_get_source_playback_state(source->_id);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_pause(AudioSource *source) {
    if (!source)
        return false;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_source_pause(source->_id);
        source->_playback_state = audio_backend_al_get_source_playback_state(source->_id);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_stop(AudioSource *source) {
    if (!source)
        return false;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_source_stop(source->_id);
        source->_playback_state = audio_backend_al_get_source_playback_state(source->_id);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_set_playback_position(AudioSource *source, UInt32 position_ms) {
    if (!source)
        return false;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        source->_playback_position = position_ms;
        audio_backend_al_set_source_playback_position(source->_id, position_ms);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_set_volume(AudioSource *source, Flt32 volume) {
    if (!source || volume < 0.0f || volume > 1.0f)
        return false;

    source->_volume = volume;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_volume(source->_id, volume);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_set_pitch(AudioSource *source, Flt32 pitch) {
    if (!source || pitch <= 0.0f)
        return false;

    source->_pitch = pitch;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_pitch(source->_id, pitch);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_fade_volume(AudioSource *source, Flt32 target_volume, UInt32 duration_ms) {
    if (!source || duration_ms == 0 || target_volume < 0.0f || target_volume > 1.0f)
        return false;

    HiResClock clock_;
    hal_hiresclock_init(&clock_);

    Flt32  vol_start_    = source->_volume;
    Flt32  vol_delta_    = (target_volume - vol_start_) / duration_ms;
    UInt32 elapsed_time_ = 0;

    while (elapsed_time_ < duration_ms) {
        source->_volume = vol_start_ + (vol_delta_ * elapsed_time_);

        switch (audio_module_state->_backend) {
        case AUDIO_BACKEND_OPENAL:
            audio_backend_al_set_source_volume(source->_id, source->_volume);
            break;

        default:
            return false;
        }

        Flt64 current_elapsed_ = hal_hiresclock_getelapsed_sec(&clock_) * 1000;
        elapsed_time_          = VT_CAST(UInt32, current_elapsed_);
    }

    // ensure that the final volume matches the target (no rounding errors)
    source->_volume = target_volume;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_volume(source->_id, target_volume);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_set_looping(AudioSource *source, Bool loop) {
    if (!source)
        return false;

    source->_loop = loop;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_looping(source->_id, loop);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_set_velocity(AudioSource *source, Flt32 x, Flt32 y, Flt32 z) {
    if (!source)
        return false;

    source->_velocity[0] = x;
    source->_velocity[1] = y;
    source->_velocity[2] = z;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_velocity(source->_id, source->_velocity[0], source->_velocity[1], source->_velocity[2]);
        return true;

    default:
        return false;
    }
}

Bool audio_utils_source_adjust_attenuation(AudioSource *source, Flt32 attenuation) {
    if (!source || attenuation < 0.0f)
        return false;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_set_source_attenuation(source->_id, attenuation);
        return true;

    default:
        return false;
    }
}

// ---------------------------------- buffer ---------------------------------- //

Bool audio_utils_buffer_reload(AudioBuffer *buffer, ConstStr audio_id) {
    if (!buffer || !audio_id)
        return false;

    if (!container_map_contains(audio_module_state->_data_map, audio_id))
        return false;

    AudioData *data_ = container_map_get(audio_module_state->_data_map, AudioData, audio_id);
    if (!data_)
        return false;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        if (!audio_backend_al_buffer_fill_data(buffer->_id, data_->_channel_format, data_->_pcm_data, data_->_data_size,
                                               data_->_sample_rate))
            return false;
        break;

    default:
        return false;
    }

    buffer->_data = data_;
    return true;
}

Bool audio_utils_buffer_bind_to_source(AudioBuffer *buffer, AudioSource *source) {
    if (!buffer || !source)
        return false;

    switch (audio_module_state->_backend) {
    case AUDIO_BACKEND_OPENAL:
        audio_backend_al_attach_buffer_to_source(source->_id, buffer->_id);
        return true;

    default:
        return false;
    }
}

ByteSize audio_utils_buffer_get_size(AudioBuffer *buffer) { return !buffer ? 0 : buffer->_data->_data_size; }

UInt32 audio_utils_buffer_get_duration(AudioBuffer *buffer) { return !buffer ? 0 : buffer->_data->_duration_ms; }

AudioFormat audio_utils_buffer_get_format(AudioBuffer *buffer) {
    return !buffer ? 0 : VT_CAST(AudioFormat, buffer->_data->_format);
}

AudioChannelFormat audio_utils_buffer_get_channel_format(AudioBuffer *buffer) {
    return !buffer ? 0 : VT_CAST(AudioChannelFormat, buffer->_data->_channel_format);
}
