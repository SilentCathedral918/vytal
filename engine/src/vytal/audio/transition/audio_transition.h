#pragma once

#include "vytal/defines/audio/audio_transition.h"
#include "vytal/defines/shared.h"

VT_API Bool audio_transition_set_volume(AudioSource *source, const Flt32 target, const UInt32 duration_ms);
VT_API Bool audio_transition_set_pitch(AudioSource *source, const Flt32 target, const UInt32 duration_ms);
VT_API Bool audio_transition_cross_set_volume(AudioSource *out_source, AudioSource *in_source, const UInt32 duration_ms);