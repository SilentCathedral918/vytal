#pragma once

#include "vytal/defines/audio/audio_transition.h"
#include "vytal/defines/core/container.h"
#include "vytal/defines/core/mem.h"

// ------------------------------ task variant ------------------------------ //

typedef enum Audio_Sequence_Task_Variant { AUDIO_SEQUENCE_TASK_PLAY, AUDIO_SEQUENCE_TASK_FADE, AUDIO_SEQUENCE_TASK_CROSSFADE, AUDIO_SEQUENCE_TASK_STOP } AudioSequenceTaskVariant;

// --------------------------------- task --------------------------------- //

typedef struct Audio_Sequence_Task {
    UInt64                   _id;
    AudioSource             *_source;
    AudioSource             *_source_other; // for cross-fading
    AudioSequenceTaskVariant _variant;
    UInt32                   _start_time_ms;
    UInt32                   _duration_ms;
    AudioTransitionData      _current_data;
    AudioTransitionData      _target_data;

    void (*_apply_transition)(AudioSource *source, AudioSource *other, AudioTransitionData current, AudioTransitionData target, Flt32 progress);
} AudioSequenceTask;

// ------------------------------- sequence ------------------------------- //

typedef struct Audio_Sequence {
    UInt64 _id;
    Array  _tasks;
    UInt32 _duration_ms;
    UInt32 _elapsed_ms;
    Bool   _retain;
} AudioSequence;
