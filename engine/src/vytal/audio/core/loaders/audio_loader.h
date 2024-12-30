#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/core/container.h"
#include "vytal/defines/shared.h"

VT_API AudioData audio_core_load_from_file(ConstStr filepath, Map data_map);
VT_API Bool      audio_core_unload_data(ConstStr id, Map data_map);
