#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/shared.h"

// ---------------------------------- source ---------------------------------- //

VT_API Bool  audio_utils_source_set_position(AudioSource *source, Flt32 x, Flt32 y, Flt32 z);
VT_API Bool  audio_utils_source_translate(AudioSource *source, Flt32 dx, Flt32 dy, Flt32 dz);
VT_API Bool  audio_utils_source_set_direction(AudioSource *source, Flt32 x, Flt32 y, Flt32 z);
VT_API Bool  audio_utils_source_reset_direction(AudioSource *source);
VT_API Bool  audio_utils_source_play(AudioSource *source);
VT_API Bool  audio_utils_source_pause(AudioSource *source);
VT_API Bool  audio_utils_source_stop(AudioSource *source);
VT_API Bool  audio_utils_source_set_playback_position(AudioSource *source, UInt32 position_ms);
VT_API Bool  audio_utils_source_set_volume(AudioSource *source, Flt32 volume);
VT_API Flt32 audio_utils_source_get_volume(AudioSource *source);
VT_API Bool  audio_utils_source_set_pitch(AudioSource *source, Flt32 pitch);
VT_API Bool  audio_utils_source_fade_volume(AudioSource *source, Flt32 target_volume, UInt32 duration_ms);
VT_API Bool  audio_utils_source_set_looping(AudioSource *source, Bool loop);
VT_API Bool  audio_utils_source_set_velocity(AudioSource *source, Flt32 x, Flt32 y, Flt32 z);
VT_API Bool  audio_utils_source_adjust_attenuation(AudioSource *source, Flt32 attenuation);

// ---------------------------------- buffer ---------------------------------- //

VT_API Bool               audio_utils_buffer_reload(AudioBuffer *buffer, ConstStr audio_id);
VT_API Bool               audio_utils_buffer_bind_to_source(AudioBuffer *buffer, AudioSource *source);
VT_API ByteSize           audio_utils_buffer_get_size(AudioBuffer *buffer);
VT_API UInt32             audio_utils_buffer_get_duration(AudioBuffer *buffer);
VT_API AudioFormat        audio_utils_buffer_get_format(AudioBuffer *buffer);
VT_API AudioChannelFormat audio_utils_buffer_get_channel_format(AudioBuffer *buffer);
