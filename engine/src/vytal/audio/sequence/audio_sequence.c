#include "audio_sequence.h"

#include "vytal/audio/core/utils/audio_utils.h"
#include "vytal/audio/module/audio.h"
#include "vytal/core/containers/array/array.h"
#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/hash/hash.h"
#include "vytal/core/memory/allocators/pool.h"
#include "vytal/core/misc/console/console.h"

#define sequences VT_CAST(Array, audio_module_get_sequences())
#define sequence_task_pool VT_CAST(PoolAllocator, audio_module_get_sequence_task_pool())

void _audio_sequence_play_callback(AudioSource *source, AudioSource *other, AudioTransitionData current,
                                   AudioTransitionData target, Flt32 progress) {
    audio_utils_source_play(source);
    audio_utils_source_set_volume(source, current._value_flt32);
}

void _audio_sequence_set_volume_callback(AudioSource *source, AudioSource *other, AudioTransitionData current,
                                         AudioTransitionData target, Flt32 progress) {
    Flt32 vol_curr_   = current._value_flt32;
    Flt32 vol_target_ = target._value_flt32;
    Flt32 vol_new_    = vol_curr_ + ((vol_target_ - vol_curr_) * progress);

    audio_utils_source_set_volume(source, vol_new_);
}

void _audio_sequence_cross_set_volume_callback(AudioSource *source, AudioSource *other, AudioTransitionData left,
                                               AudioTransitionData right, Flt32 progress) {
    Flt32 vol_out_curr_   = 1.0f;
    Flt32 vol_out_target_ = 0.0f;
    Flt32 vol_out_new_    = vol_out_curr_ + ((vol_out_target_ - vol_out_curr_) * progress);

    Flt32 vol_in_curr_   = 0.0f;
    Flt32 vol_in_target_ = 1.0f;
    Flt32 vol_in_new_    = vol_in_curr_ + ((vol_in_target_ - vol_in_curr_) * progress);

    audio_utils_source_set_volume(source, vol_out_new_);
    audio_utils_source_set_volume(other, vol_in_new_);
}

void _audio_sequence_stop_callback(AudioSource *source, AudioSource *other, AudioTransitionData current,
                                   AudioTransitionData target, Flt32 progress) {
    audio_utils_source_stop(source);
}

Int32 _audio_sequence_compare_task_start_time(const void *left, const void *right) {
    AudioSequenceTask *task_left_  = VT_CAST(AudioSequenceTask *, left);
    AudioSequenceTask *task_right_ = VT_CAST(AudioSequenceTask *, right);

    return (task_left_->_start_time_ms < task_right_->_start_time_ms)   ? -1
           : (task_left_->_start_time_ms > task_right_->_start_time_ms) ? 1
                                                                        : 0;
}

AudioSequenceTask *_audio_sequence_search_task(const Array tasks, const UInt64 id) {
    for (ByteSize i = 0; i < container_array_length(tasks); ++i) {
        AudioSequenceTask *task_ = container_array_get_at_index(tasks, i);
        if (!task_)
            continue;

        if (task_->_id == id)
            return task_;
    }

    return NULL;
}

AudioSequence *_audio_sequence_search(const UInt64 id) {
    for (ByteSize i = 0; i < container_array_length(sequences); ++i) {
        AudioSequence *sequence_ = container_array_get_at_index(sequences, i);
        if (!sequence_)
            continue;

        if (sequence_->_id == id)
            return sequence_;
    }

    return NULL;
}

void _audio_sequence_update_duration(AudioSequence *sequence) {
    UInt32 max_duration_ = 0;

    for (ByteSize i = 0; i < container_array_length(sequence->_tasks); ++i) {
        AudioSequenceTask *task_ = container_array_get_at_index(sequence->_tasks, i);
        if (!task_)
            continue;

        UInt32 task_end_time_ = task_->_start_time_ms + task_->_duration_ms;
        if (task_end_time_ > max_duration_)
            max_duration_ = task_end_time_;
    }

    sequence->_duration_ms = max_duration_;
}

AudioSequence *audio_sequence_construct(ConstStr id, const Bool retain) {
    const UInt64   id_             = hash_hashstr(id, HASH_MODE_XX64);
    AudioSequence *check_sequence_ = _audio_sequence_search(id_);
    if (check_sequence_)
        return NULL; // sequence of specified id already exists

    const ByteSize chunk_size_ = allocator_pool_chunksize(sequence_task_pool);
    AudioSequence  sequence_   = {
           ._id          = id_,
           ._tasks       = container_array_construct_custom(AudioSequenceTask, sequence_task_pool, ALLOCTYPE_POOL, chunk_size_),
           ._duration_ms = 0,
           ._elapsed_ms  = 0,
           ._retain      = retain};

    container_array_push(sequences, AudioSequence, sequence_);
    return container_array_get_at_index(sequences, container_array_length(sequences) - 1);
}

Bool audio_sequence_destruct(AudioSequence *sequence) {
    if (!sequence)
        return false;

    if (!sequence->_tasks)
        return false;

    if (!container_array_destruct(sequence->_tasks))
        return false;

    hal_mem_memzero(sequence, sizeof(AudioSequence));
    return container_array_remove(sequences, sequence);
}

Bool audio_sequence_add_task(AudioSequence *sequence, ConstStr id, AudioSequenceTaskVariant variant, AudioSource *source,
                             AudioSource *other, const Flt32 start_time_ms, const Flt32 duration_ms,
                             const AudioTransitionData current, const AudioTransitionData target) {
    if (!sequence || !id || !source)
        return false;

    const UInt64 id_ = hash_hashstr(id, HASH_MODE_XX64);

    AudioSequenceTask *check_task_ = _audio_sequence_search_task(sequence->_tasks, id_);
    if (check_task_)
        return false; // task of specified id already exists

    AudioSequenceTask task_ = {0};
    task_._id               = id_;
    task_._source           = source;
    task_._duration_ms      = duration_ms;

    UInt32 start_time_ = start_time_ms;
    if (start_time_ < sequence->_elapsed_ms)
        start_time_ = sequence->_elapsed_ms;

    task_._start_time_ms = start_time_;

    switch (variant) {
    case AUDIO_SEQUENCE_TASK_PLAY:
        task_._current_data._value_flt32 = current._value_flt32;
        task_._apply_transition          = _audio_sequence_play_callback;
        break;

    case AUDIO_SEQUENCE_TASK_FADE:
        task_._current_data._value_flt32 = current._value_flt32;
        task_._target_data._value_flt32  = target._value_flt32;
        task_._apply_transition          = _audio_sequence_set_volume_callback;
        break;

    case AUDIO_SEQUENCE_TASK_CROSSFADE:
        if (!other)
            return false;
        task_._source_other              = other;
        task_._current_data._value_flt32 = current._value_flt32;
        task_._target_data._value_flt32  = target._value_flt32;
        task_._apply_transition          = _audio_sequence_cross_set_volume_callback;
        break;

    case AUDIO_SEQUENCE_TASK_STOP:
        task_._apply_transition = _audio_sequence_stop_callback;
        break;

    default:
        return false;
    }

    container_array_push(sequence->_tasks, AudioSequenceTask, task_);
    container_array_sort(sequence->_tasks, _audio_sequence_compare_task_start_time);
    _audio_sequence_update_duration(sequence);
    return true;
}

Bool audio_sequence_remove_task(AudioSequence *sequence, ConstStr id) {
    if (!sequence || !id)
        return false;

    const UInt64       id_   = hash_hashstr(id, HASH_MODE_XX64);
    AudioSequenceTask *task_ = _audio_sequence_search_task(sequence->_tasks, id_);
    if (!task_)
        return false;

    audio_utils_source_stop(task_->_source);
    if (task_->_source_other)
        audio_utils_source_stop(task_->_source_other);

    container_array_remove(sequence->_tasks, task_);
    _audio_sequence_update_duration(sequence);
    return true;
}

AudioSequenceTask *audio_sequence_get_task(AudioSequence *sequence, ConstStr id) {
    if (!sequence || !id)
        return NULL;

    const UInt64 id_ = hash_hashstr(id, HASH_MODE_XX64);
    return _audio_sequence_search_task(sequence->_tasks, id_);
}

AudioSequence *audio_sequence_get(ConstStr id) {
    if (!id)
        return NULL;

    const UInt64 id_ = hash_hashstr(id, HASH_MODE_XX64);
    return _audio_sequence_search(id_);
}
