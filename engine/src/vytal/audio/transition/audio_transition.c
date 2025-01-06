#include "audio_transition.h"

#include "vytal/audio/core/utils/audio_utils.h"
#include "vytal/audio/module/audio.h"
#include "vytal/core/containers/array/array.h"
#include "vytal/core/misc/console/console.h"

void _audio_transition_set_volume_callback(AudioSource *source, AudioTransitionData current, AudioTransitionData target,
                                           Flt32 progress) {
    Flt32 vol_curr_   = current._value_flt32;
    Flt32 vol_target_ = target._value_flt32;
    Flt32 vol_new_    = vol_curr_ + ((vol_target_ - vol_curr_) * progress);

    // Debugging log to check the values
    misc_console_writeln("Transitioning Volume: curr=%.4f, target=%.4f, progress=%.4f, new=%.4f", vol_curr_, vol_target_,
                         progress, vol_new_);

    audio_utils_source_set_volume(source, vol_new_);
}

Bool audio_transition_set_volume(AudioSource *source, const Flt32 target, const UInt32 duration_ms) {
    if (!source || target > 1.0f || target < 0.0f || duration_ms == 0)
        return false;

    Array transition_tasks_ = VT_CAST(Array, audio_module_get_transition_tasks());

    AudioTransitionTask task_ = {._source           = source,
                                 ._current_data     = {._value_flt32 = source->_volume},
                                 ._target_data      = {._value_flt32 = target},
                                 ._data_size        = sizeof(source->_volume),
                                 ._duration_ms      = duration_ms,
                                 ._elapsed_ms       = 0,
                                 ._apply_transition = _audio_transition_set_volume_callback};

    container_array_push(transition_tasks_, AudioTransitionTask, task_);
    return true;
}
