#include "audio_loader.h"

#include "vytal/audio/core/loaders/wav/wav_loader.h"
#include "vytal/core/containers/map/map.h"
#include "vytal/core/logger/logger.h"
#include "vytal/core/misc/assertions/assertions.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/misc/string/vtstr.h"

ConstStr supported_formats[]        = {".wav", ".ogg", ".flac"};
ByteSize supported_format_lengths[] = {4, 4, 5};

Int32 _audio_core_detect_format(ConstStr filepath) {
    ByteSize path_length_ = misc_str_strlen(filepath);

    for (ByteSize i = 0; i < VT_STATIC_ARRAY_SIZE(supported_formats); ++i) {
        if (path_length_ <= supported_format_lengths[i])
            continue;

        if (misc_str_strncmp(filepath + path_length_ - supported_format_lengths[i], supported_formats[i],
                             supported_format_lengths[i], true) == 0)
            return i;
    }

    return -1;
}

AudioData audio_core_load_from_file(ConstStr filepath, Map data_map) {
    if (!filepath || !data_map)
        return VT_STRUCT(AudioData, 0);

    Int32 format_ = _audio_core_detect_format(filepath);
    if (format_ == -1) {
        VT_LOG_ERROR("Engine", "Unsupported audio format _ filepath: %s", filepath);
        return VT_STRUCT(AudioData, 0);
    }

    AudioData data_ = VT_STRUCT(AudioData, 0);

    switch (format_) {
    case AUDIO_FILE_FORMAT_WAV:
        data_ = audio_core_wav_load_from_file(filepath);

        ConstStr filename = platform_fs_get_filename_from_path(filepath);
        container_map_insert(data_map, VT_CAST(VoidPtr, filename), &data_);

        break;

    case AUDIO_FILE_FORMAT_OGG:
        break;

    case AUDIO_FILE_FORMAT_FLAC:
        break;

    default:
        break;
    }

    return data_;
}

Bool audio_core_unload_data(ConstStr id, Map data_map) {
    if (!id || !data_map)
        return false;

    AudioData *data_ = container_map_get(data_map, AudioData, id);
    if (!data_)
        return false;

    switch (data_->_format) {
    case AUDIO_FILE_FORMAT_WAV:
        return audio_core_wav_unload_data(data_);

    case AUDIO_FILE_FORMAT_OGG:
        break;

    case AUDIO_FILE_FORMAT_FLAC:
        break;

    default:
        break;
    }

    return false;
}
