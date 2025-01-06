#include "audio.h"
#include "vytal/audio/backend/openal/audio_openal.h"
#include "vytal/audio/core/utils/audio_utils.h"
#include "vytal/audio/module/audio.h"

Bool audio_load(ConstStr id, ConstStr filepath) {
    if (!id || !filepath)
        return false;

    if (audio_module_get_audio(id))
        return true; // audio data already loaded

    if (!audio_module_load_audio(id, filepath))
        return false;

    return (audio_module_construct_buffer(id, id) != NULL);
}

Bool audio_unload(ConstStr id) {
    if (!id)
        return false;

    return audio_module_unload_audio(id);
}

Bool audio_play(ConstStr id, const Bool loop) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (source_)
        return audio_utils_source_set_looping(source_, loop) && audio_utils_source_play(source_);

    if (!audio_module_get_audio(id) && !audio_module_load_audio(id, NULL))
        return false;

    AudioBuffer *buffer_ = audio_module_get_buffer(id);
    if (!buffer_ && !audio_module_construct_buffer(id, id))
        return false;

    AudioSource *new_source_ = audio_module_construct_source_with_buffer(id, id, loop);
    return new_source_ ? audio_utils_source_play(new_source_) : false;
}

Bool audio_pause(ConstStr id) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (!source_)
        return false;

    return audio_utils_source_pause(source_);
}

Bool audio_stop(ConstStr id) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (!source_)
        return false;

    return audio_utils_source_stop(source_);
}

Bool audio_set_position(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (!source_)
        return false;

    return audio_utils_source_set_position(source_, x, y, z);
}

Bool audio_set_velocity(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (!source_)
        return false;

    return audio_utils_source_set_velocity(source_, x, y, z);
}

Bool audio_set_direction(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (!source_)
        return false;

    return audio_utils_source_set_direction(source_, x, y, z);
}

Bool audio_translate(ConstStr id, const Flt32 delta_x, const Flt32 delta_y, const Flt32 delta_z) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (!source_)
        return false;

    return audio_utils_source_translate(source_, delta_x, delta_y, delta_z);
}

Bool audio_translate_by_velocity(ConstStr id) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (!source_)
        return false;

    return audio_utils_source_translate(source_, source_->_velocity[0], source_->_velocity[1], source_->_velocity[2]);
}

Bool audio_set_volume(ConstStr id, const Flt32 volume) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (!source_)
        return false;

    return audio_utils_source_set_volume(source_, volume);
}

Bool audio_set_pitch(ConstStr id, const Flt32 pitch) {
    if (!id)
        return false;

    AudioSource *source_ = audio_module_get_source(id);
    if (!source_)
        return false;

    return audio_utils_source_set_pitch(source_, pitch);
}
