#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/audio/audio_wav.h"
#include "vytal/defines/shared.h"

VT_API AudioData audio_core_wav_load_from_file(ConstStr filepath);
VT_API Bool      audio_core_wav_unload_data(AudioData *data);