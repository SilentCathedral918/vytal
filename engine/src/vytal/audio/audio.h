#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/audio/audio_sequence.h"
#include "vytal/defines/shared.h"

VT_API Bool           audio_load(ConstStr id, ConstStr filepath);
VT_API Bool           audio_unload(ConstStr id);
VT_API Bool           audio_play(ConstStr id, const Bool loop);
VT_API Bool           audio_pause(ConstStr id);
VT_API Bool           audio_stop(ConstStr id);
VT_API AudioSource   *audio_create_source(ConstStr id, ConstStr buffer_id);
VT_API AudioSource   *audio_create_empty_source(ConstStr id);
VT_API AudioSource   *audio_create_source_from_file(ConstStr id, ConstStr filepath);
VT_API Bool           audio_remove_source(ConstStr id);
VT_API AudioSource   *audio_get_source(ConstStr id);
VT_API AudioBuffer   *audio_create_buffer(ConstStr id, ConstStr audio_id);
VT_API AudioBuffer   *audio_create_buffer_from_file(ConstStr id, ConstStr filepath);
VT_API Bool           audio_remove_buffer(ConstStr id);
VT_API AudioBuffer   *audio_get_buffer(ConstStr id);
VT_API Bool           audio_set_position(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z);
VT_API Bool           audio_set_velocity(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z);
VT_API Bool           audio_set_direction(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z);
VT_API Bool           audio_translate(ConstStr id, const Flt32 delta_x, const Flt32 delta_y, const Flt32 delta_z);
VT_API Bool           audio_translate_by_velocity(ConstStr id);
VT_API Bool           audio_set_volume(ConstStr id, const Flt32 volume);
VT_API Bool           audio_set_pitch(ConstStr id, const Flt32 pitch);
VT_API Bool           audio_interpolate_volume(ConstStr id, const Flt32 target, const UInt32 duration_ms);
VT_API Bool           audio_interpolate_pitch(ConstStr id, const Flt32 target, const UInt32 duration_ms);
VT_API Bool           audio_mute(ConstStr id);
VT_API Bool           audio_set_looping(ConstStr id, const Bool loop);
VT_API Bool           audio_cross_interpolate_volume(ConstStr out_audio, ConstStr in_audio, const UInt32 duration_ms);
VT_API AudioSequence *audio_create_sequence(ConstStr id, const Bool retain);
VT_API Bool           audio_remove_sequence(ConstStr id);
VT_API AudioSequence *audio_get_sequence(ConstStr id);
VT_API Bool           audio_create_sequence_task_play(ConstStr sequence_id, ConstStr task_id, ConstStr source_id, const UInt32 start_time_ms, const Flt32 volume);
VT_API Bool           audio_create_sequence_task_fade(ConstStr sequence_id, ConstStr task_id, ConstStr source_id, const UInt32 start_time_ms, const UInt32 duration_ms, const Flt32 current_volume, const Flt32 target_volume);
VT_API Bool           audio_create_sequence_task_crossfade(ConstStr sequence_id, ConstStr task_id, ConstStr left_source_id, ConstStr right_source_id, const UInt32 start_time_ms, const UInt32 duration_ms, const Flt32 left_volume, const Flt32 right_volume);
VT_API Bool           audio_create_sequence_task_stop(ConstStr sequence_id, ConstStr task_id, ConstStr source_id, const UInt32 start_time_ms);
