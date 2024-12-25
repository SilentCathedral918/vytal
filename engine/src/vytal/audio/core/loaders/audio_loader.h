#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/shared.h"

VT_API AudioData audio_core_load_from_file(ConstStr filepath);
VT_API Bool      audio_core_unload_data(AudioData *data);
