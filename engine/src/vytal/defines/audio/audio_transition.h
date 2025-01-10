#pragma once

#include "vytal/defines/audio/audio.h"
#include "vytal/defines/core/types.h"

// ------------------------------ transition data ------------------------------ //

typedef struct Audio_Transition_Data {
    Flt32 _value_flt32;
} AudioTransitionData;

// ----------------------------------- task ----------------------------------- //

typedef struct Audio_Transition_Task {
    AudioSource        *_source;
    AudioTransitionData _current_data;
    AudioTransitionData _target_data;
    ByteSize            _data_size;
    Flt32               _duration_ms;
    Flt32               _elapsed_ms;

    void (*_apply_transition)(AudioSource *source, AudioTransitionData current, AudioTransitionData target, Flt32 progress);
} AudioTransitionTask;
