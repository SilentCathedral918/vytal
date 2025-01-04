#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/shared.h"

// --------------------------------- device --------------------------------- //

AudioDevice audio_backend_al_open_device(ConstStr device_name);
Bool        audio_backend_al_close_device(AudioDevice *device);
Bool        audio_backend_al_get_context_device(AudioDevice *out_device, const AudioContext *context);

// --------------------------------- context --------------------------------- //

AudioContext audio_backend_al_create_context(const AudioDevice *device, const Int32 *attr_list);
Bool         audio_backend_al_destroy_context(AudioContext *context);
Bool         audio_backend_al_suspend_context(AudioContext *context);
Bool         audio_backend_al_make_context_current(VoidPtr context);
Bool         audio_backend_al_process_context(AudioContext *context);
Bool         audio_backend_al_get_current_context(AudioContext *out_context);

// --------------------------------- listener --------------------------------- //

Bool audio_backend_al_set_listener_position(AudioListener *listener, const Flt32 x, const Flt32 y, const Flt32 z);
Bool audio_backend_al_set_listener_velocity(AudioListener *listener, const Flt32 x, const Flt32 y, const Flt32 z);
Bool audio_backend_al_set_listener_orientation(AudioListener *listener, Flt32 *orientation);

// --------------------------------- source --------------------------------- //

Bool               audio_backend_al_generate_source(UInt32 *source);
Bool               audio_backend_al_generate_sources(UInt32 *sources, const ByteSize count);
Bool               audio_backend_al_delete_source(UInt32 *source);
Bool               audio_backend_al_delete_sources(UInt32 *sources, const ByteSize count);
Bool               audio_backend_al_get_source_position(UInt32 source, Flt32 *out_position);
void               audio_backend_al_set_source_position(UInt32 source, const Flt32 x, const Flt32 y, const Flt32 z);
Bool               audio_backend_al_get_source_velocity(UInt32 source, Flt32 *out_velocity);
void               audio_backend_al_set_source_velocity(UInt32 source, const Flt32 x, const Flt32 y, const Flt32 z);
Bool               audio_backend_al_get_source_direction(UInt32 source, Flt32 *out_direction);
Bool               audio_backend_al_set_source_direction(UInt32 source, const Flt32 *direction, const Bool omnidirectional);
void               audio_backend_al_attach_buffer_to_source(UInt32 source, const UInt32 buffer);
void               audio_backend_al_source_play(const UInt32 source);
void               audio_backend_al_source_pause(const UInt32 source);
void               audio_backend_al_source_stop(const UInt32 source);
UInt32             audio_backend_al_get_source_playback_position(const UInt32 source);
void               audio_backend_al_set_source_playback_position(const UInt32 source, const UInt32 position_ms);
AudioPlaybackState audio_backend_al_get_source_playback_state(const UInt32 source);
Flt32              audio_backend_al_get_source_volume(const UInt32 source);
void               audio_backend_al_set_source_volume(const UInt32 source, const Flt32 volume);
Flt32              audio_backend_al_get_source_attenuation(const UInt32 source);
void               audio_backend_al_set_source_attenuation(const UInt32 source, const Flt32 attenuation);
Bool               audio_backend_al_is_source_looping(const UInt32 source);
void               audio_backend_al_set_source_looping(const UInt32 source, const Bool loop);
Bool               audio_backend_al_is_source_omnidirectional(const UInt32 source);
void               audio_backend_al_set_source_omnidirectional(const UInt32 source, const Bool omnidirectional);
Flt32              audio_backend_al_get_source_pitch(const UInt32 source);
void               audio_backend_al_set_source_pitch(const UInt32 source, const Flt32 pitch);

// --------------------------------- buffer --------------------------------- //

Bool audio_backend_al_generate_buffer(UInt32 *buffer);
Bool audio_backend_al_generate_buffers(UInt32 *buffers, const ByteSize count);
Bool audio_backend_al_delete_buffer(UInt32 *buffer);
Bool audio_backend_al_delete_buffers(UInt32 *buffers, const ByteSize count);
Bool audio_backend_al_buffer_fill_data(const UInt32 buffer, const AudioChannelFormat channel_format, const VoidPtr data,
                                       const ByteSize size, const ByteSize sample_rate);
Bool audio_backend_al_is_buffer_valid(const UInt32 buffer);