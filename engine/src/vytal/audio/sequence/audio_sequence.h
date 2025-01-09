#pragma once

#include "vytal/defines/audio/audio_sequence.h"
#include "vytal/defines/audio/audio_transition.h"
#include "vytal/defines/shared.h"

VT_API AudioSequence     *audio_sequence_construct(ConstStr id, const Bool retain);
VT_API Bool               audio_sequence_destruct(AudioSequence *sequence);
VT_API Bool               audio_sequence_add_task(AudioSequence *sequence, ConstStr id, AudioSequenceTaskVariant variant, AudioSource *source, AudioSource *other, const Flt32 start_time_ms, const Flt32 duration_ms, const AudioTransitionData current,
                                                  const AudioTransitionData target);
VT_API Bool               audio_sequence_remove_task(AudioSequence *sequence, ConstStr id);
VT_API AudioSequenceTask *audio_sequence_get_task(AudioSequence *sequence, ConstStr id);
VT_API AudioSequence     *audio_sequence_get(ConstStr id);