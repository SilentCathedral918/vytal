#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/shared.h"

VT_API ByteSize audio_module_get_size(void);

VT_API Bool audio_module_startup(VoidPtr module);
VT_API Bool audio_module_shutdown(void);
VT_API Bool audio_module_update(void);
VT_API Bool audio_module_play_audio(ConstStr filepath);
