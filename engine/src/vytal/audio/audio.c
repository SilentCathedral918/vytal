#include "audio.h"
#include "vytal/audio/backend/openal/audio_openal.h"
#include "vytal/audio/core/utils/audio_utils.h"
#include "vytal/audio/module/audio.h"
#include "vytal/audio/sequence/audio_sequence.h"
#include "vytal/audio/transition/audio_transition.h"

Bool audio_load(ConstStr id, ConstStr filepath) {
    if (!id || !filepath)
        return false;

    if (audio_module_get_audio(id))
        return true; // audio data already loaded

    if (!audio_module_load_audio(id, filepath))
        return false;

    return (audio_module_construct_buffer(id, id) != NULL);
}
Bool         audio_unload(ConstStr id) { return audio_module_unload_audio(id); }
AudioSource *audio_create_source(ConstStr id, ConstStr buffer_id) { return audio_module_construct_source_with_buffer(id, buffer_id, false); }
AudioSource *audio_create_empty_source(ConstStr id) { return audio_module_construct_source(id, false); }
AudioSource *audio_create_source_from_file(ConstStr id, ConstStr filepath) {
    AudioData *data_ = audio_module_get_audio(id);
    if (!data_)
        data_ = audio_module_load_audio(id, filepath);

    AudioBuffer *buffer_ = audio_module_get_buffer(id);
    if (!buffer_)
        buffer_ = audio_module_construct_buffer(id, id);

    if (!data_ || !buffer_)
        return NULL;
    else
        return audio_module_construct_source_with_buffer(id, id, false);
}
Bool         audio_remove_source(ConstStr id) { return audio_module_destruct_source(id); }
AudioBuffer *audio_create_buffer(ConstStr id, ConstStr audio_id) { return audio_module_construct_buffer(id, audio_id); }
AudioBuffer *audio_create_buffer_from_file(ConstStr id, ConstStr filepath) {
    AudioData *data_ = audio_module_get_audio(id);
    if (!data_)
        data_ = audio_module_load_audio(id, filepath);

    return audio_module_construct_buffer(id, id);
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
Bool audio_pause(ConstStr id) { return audio_utils_source_pause(audio_module_get_source(id)); }
Bool audio_stop(ConstStr id) { return audio_utils_source_stop(audio_module_get_source(id)); }
Bool audio_set_position(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z) { return audio_utils_source_set_position(audio_module_get_source(id), x, y, z); }
Bool audio_set_velocity(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z) { return audio_utils_source_set_velocity(audio_module_get_source(id), x, y, z); }
Bool audio_set_direction(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z) { return audio_utils_source_set_direction(audio_module_get_source(id), x, y, z); }
Bool audio_translate(ConstStr id, const Flt32 delta_x, const Flt32 delta_y, const Flt32 delta_z) { return audio_utils_source_translate(audio_module_get_source(id), delta_x, delta_y, delta_z); }
Bool audio_translate_by_velocity(ConstStr id) {
    AudioSource *source_ = audio_module_get_source(id);
    return !source_ ? false : audio_utils_source_translate(source_, source_->_velocity[0], source_->_velocity[1], source_->_velocity[2]);
}
Bool           audio_set_volume(ConstStr id, const Flt32 volume) { return audio_utils_source_set_volume(audio_module_get_source(id), volume); }
Bool           audio_set_pitch(ConstStr id, const Flt32 pitch) { return audio_utils_source_set_pitch(audio_module_get_source(id), pitch); }
Bool           audio_interpolate_volume(ConstStr id, const Flt32 target, const UInt32 duration_ms) { return audio_transition_set_volume(audio_module_get_source(id), target, duration_ms); }
Bool           audio_interpolate_pitch(ConstStr id, const Flt32 target, const UInt32 duration_ms) { return audio_transition_set_pitch(audio_module_get_source(id), target, duration_ms); }
Bool           audio_mute(ConstStr id) { return audio_utils_source_set_volume(audio_module_get_source(id), 0.0f); }
Bool           audio_set_looping(ConstStr id, const Bool loop) { return audio_utils_source_set_looping(audio_module_get_source(id), loop); }
Bool           audio_cross_interpolate_volume(ConstStr out_audio, ConstStr in_audio, const UInt32 duration_ms) { return audio_transition_cross_set_volume(audio_module_get_source(out_audio), audio_module_get_source(in_audio), duration_ms); }
AudioSource   *audio_get_source(ConstStr id) { return audio_module_get_source(id); }
AudioBuffer   *audio_get_buffer(ConstStr id) { return audio_module_get_buffer(id); }
AudioSequence *audio_create_sequence(ConstStr id, const Bool retain) { return audio_sequence_construct(id, retain); }
Bool           audio_remove_sequence(ConstStr id) { return audio_sequence_destruct(audio_sequence_get(id)); }
AudioSequence *audio_get_sequence(ConstStr id) { return audio_sequence_get(id); }
Bool           audio_create_sequence_task_play(ConstStr sequence_id, ConstStr task_id, ConstStr source_id, const UInt32 start_time_ms, const Flt32 volume) {
    AudioSequence *sequence_ = audio_sequence_get(sequence_id);
    if (!sequence_)
        sequence_ = audio_sequence_construct(sequence_id, false);

    AudioSource *source_ = audio_get_source(source_id);
    if (!source_)
        source_ = audio_create_source(source_id, source_id);

    return audio_sequence_add_task(sequence_, task_id, AUDIO_SEQUENCE_TASK_PLAY, source_, NULL, start_time_ms, 1, VT_STRUCT(AudioTransitionData, ._value_flt32 = volume), VT_STRUCT(AudioTransitionData, ._value_flt32 = 0.0f));
}
Bool audio_create_sequence_task_fade(ConstStr sequence_id, ConstStr task_id, ConstStr source_id, const UInt32 start_time_ms, const UInt32 duration_ms, const Flt32 current_volume, const Flt32 target_volume) {
    AudioSequence *sequence_ = audio_sequence_get(sequence_id);
    if (!sequence_)
        sequence_ = audio_sequence_construct(sequence_id, false);

    AudioSource *source_ = audio_get_source(source_id);
    if (!source_)
        source_ = audio_create_source(source_id, source_id);

    return audio_sequence_add_task(sequence_, task_id, AUDIO_SEQUENCE_TASK_FADE, source_, NULL, start_time_ms, duration_ms, VT_STRUCT(AudioTransitionData, ._value_flt32 = current_volume), VT_STRUCT(AudioTransitionData, ._value_flt32 = target_volume));
}
Bool audio_create_sequence_task_crossfade(ConstStr sequence_id, ConstStr task_id, ConstStr left_source_id, ConstStr right_source_id, const UInt32 start_time_ms, const UInt32 duration_ms, const Flt32 left_volume, const Flt32 right_volume) {
    AudioSequence *sequence_ = audio_sequence_get(sequence_id);
    if (!sequence_)
        sequence_ = audio_sequence_construct(sequence_id, false);

    AudioSource *left_source_ = audio_get_source(left_source_id);
    if (!left_source_)
        left_source_ = audio_create_source(left_source_id, left_source_id);

    AudioSource *right_source_ = audio_get_source(right_source_id);
    if (!right_source_)
        right_source_ = audio_create_source(right_source_id, right_source_id);

    return audio_sequence_add_task(sequence_, task_id, AUDIO_SEQUENCE_TASK_CROSSFADE, left_source_, right_source_, start_time_ms, duration_ms, VT_STRUCT(AudioTransitionData, ._value_flt32 = left_volume),
                                   VT_STRUCT(AudioTransitionData, ._value_flt32 = right_volume));
}
Bool audio_create_sequence_task_stop(ConstStr sequence_id, ConstStr task_id, ConstStr source_id, const UInt32 start_time_ms) {
    AudioSequence *sequence_ = audio_sequence_get(sequence_id);
    if (!sequence_)
        sequence_ = audio_sequence_construct(sequence_id, false);

    AudioSource *source_ = audio_get_source(source_id);
    if (!source_)
        source_ = audio_create_source(source_id, source_id);

    return audio_sequence_add_task(sequence_, task_id, AUDIO_SEQUENCE_TASK_STOP, source_, NULL, start_time_ms, 1, VT_STRUCT(AudioTransitionData, ._value_flt32 = 0.0f), VT_STRUCT(AudioTransitionData, ._value_flt32 = 0.0f));
}
