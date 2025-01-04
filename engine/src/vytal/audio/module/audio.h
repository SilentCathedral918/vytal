#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/core/container.h"
#include "vytal/defines/shared.h"

VT_API ByteSize audio_module_get_size(void);

VT_API Bool         audio_module_startup(VoidPtr module);
VT_API Bool         audio_module_shutdown(void);
VT_API Bool         audio_module_update(void);
VT_API AudioData   *audio_module_load_audio(ConstStr id, ConstStr filepath);
VT_API Bool         audio_module_unload_audio(ConstStr id);
VT_API AudioData   *audio_module_get_audio(ConstStr id);
VT_API AudioBuffer *audio_module_construct_buffer(ConstStr buffer_id, ConstStr audio_id);
VT_API AudioSource *audio_module_construct_source(ConstStr id, const Bool loop);
VT_API AudioSource *audio_module_construct_source_with_buffer(ConstStr source_id, ConstStr buffer_id, const Bool loop);
VT_API Bool         audio_module_destruct_buffer(ConstStr id);
VT_API Bool         audio_module_destruct_source(ConstStr id);
VT_API AudioBuffer *audio_module_get_buffer(ConstStr id);
VT_API AudioSource *audio_module_get_source(ConstStr id);
VT_API VoidPtr      audio_module_get_state(void);
