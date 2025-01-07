#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/shared.h"

VT_API Bool audio_load(ConstStr id, ConstStr filepath);
VT_API Bool audio_unload(ConstStr id);
VT_API Bool audio_play(ConstStr id, const Bool loop);
VT_API Bool audio_pause(ConstStr id);
VT_API Bool audio_stop(ConstStr id);
VT_API Bool audio_set_position(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z);
VT_API Bool audio_set_velocity(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z);
VT_API Bool audio_set_direction(ConstStr id, const Flt32 x, const Flt32 y, const Flt32 z);
VT_API Bool audio_translate(ConstStr id, const Flt32 delta_x, const Flt32 delta_y, const Flt32 delta_z);
VT_API Bool audio_translate_by_velocity(ConstStr id);
VT_API Bool audio_set_volume(ConstStr id, const Flt32 volume);
VT_API Bool audio_set_pitch(ConstStr id, const Flt32 pitch);
VT_API Bool audio_interpolate_volume(ConstStr id, const Flt32 target, const UInt32 duration_ms);
VT_API Bool audio_interpolate_pitch(ConstStr id, const Flt32 target, const UInt32 duration_ms);
VT_API Bool audio_cross_interpolate_volume(ConstStr out_audio, ConstStr in_audio, const UInt32 duration_ms);
