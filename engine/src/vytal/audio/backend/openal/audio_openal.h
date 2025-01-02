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
void               audio_backend_al_set_source_position(UInt32 source, const Flt32 x, const Flt32 y, const Flt32 z);
void               audio_backend_al_set_source_velocity(UInt32 source, const Flt32 x, const Flt32 y, const Flt32 z);
Bool               audio_backend_al_set_source_direction(UInt32 source, const Flt32 *direction, const Bool omnidirectional);
void               audio_backend_al_attach_buffer_to_source(UInt32 source, const UInt32 buffer);
void               audio_backend_al_source_play(const UInt32 source);
AudioPlaybackState audio_backend_al_get_playback_state(const UInt32 source);

// --------------------------------- buffer --------------------------------- //

Bool audio_backend_al_generate_buffer(UInt32 *buffer);
Bool audio_backend_al_generate_buffers(UInt32 *buffers, const ByteSize count);
Bool audio_backend_al_delete_buffer(UInt32 *buffer);
Bool audio_backend_al_delete_buffers(UInt32 *buffers, const ByteSize count);
Bool audio_backend_al_buffer_fill_data(const UInt32 buffer, const AudioChannelFormat channel_format, const VoidPtr data,
                                       const ByteSize size, const ByteSize sample_rate);
Bool audio_backend_al_is_buffer_valid(const UInt32 buffer);