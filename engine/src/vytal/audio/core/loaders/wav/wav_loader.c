#include "wav_loader.h"

#include "vytal/core/hal/memory/vtmem.h"
#include "vytal/core/logger/logger.h"
#include "vytal/core/misc/console/console.h"
#include "vytal/core/misc/string/vtstr.h"
#include "vytal/core/platform/filesystem/filesystem.h"
#include "vytal/managers/memory/memmgr.h"

AudioData audio_core_wav_load_from_file(ConstStr filepath) {
    FileHandle handle_;

    if (!platform_fs_open_file(&handle_, filepath, FILE_IO_MODE_READ, FILE_MODE_BINARY)) {
        VT_LOG_ERROR("Engine", "Failed to open WAV file _ filepath: %s", filepath);
        return VT_STRUCT(AudioData, 0);
    }

    // read the RIFF chunk
    AudioWavRiffChunk riff_chunk_;
    {
        if (!platform_fs_file_read_data(&handle_, sizeof(AudioWavRiffChunk), NULL, VT_CAST(VoidPtr, &riff_chunk_))) {
            VT_LOG_ERROR("Engine", "%s", "Failed to read RIFF chunk.");

            platform_fs_close_file(&handle_);
            return VT_STRUCT(AudioData, 0);
        }

        if (misc_str_strncmp(riff_chunk_._riff, "RIFF", 4, true) != 0 ||
            misc_str_strncmp(riff_chunk_._wave, "WAVE", 4, true) != 0) {
            VT_LOG_ERROR("Engine", "Invalid or unsupported WAV file format _ filepath: %s", filepath);

            platform_fs_close_file(&handle_);
            return VT_STRUCT(AudioData, 0);
        }
    }

    // seek to the 'fmt ' chunk
    platform_fs_seek_to_position(&handle_, sizeof(AudioWavRiffChunk));

    // read the 'fmt ' chunk
    AudioWavFmtChunk fmt_chunk_;
    {
        if (!platform_fs_file_read_data(&handle_, sizeof(AudioWavFmtChunk), NULL, VT_CAST(VoidPtr, &fmt_chunk_))) {
            VT_LOG_ERROR("Engine", "%s", "Failed to read 'fmt ' chunk.");

            platform_fs_close_file(&handle_);
            return VT_STRUCT(AudioData, 0);
        }

        if (misc_str_strncmp(fmt_chunk_._id, "fmt ", 4, true) != 0) {
            VT_LOG_ERROR("Engine", "Missing or invalid 'fmt ' chunk _ filepath: %s", filepath);

            platform_fs_close_file(&handle_);
            return VT_STRUCT(AudioData, 0);
        }

        // validate PCM format (format = 1)
        if (fmt_chunk_._format != 1) {
            VT_LOG_ERROR("Engine", "Only PCM WAV files (format = 1) are supported _ found format: %u", fmt_chunk_._format);

            platform_fs_close_file(&handle_);
            return VT_STRUCT(AudioData, 0);
        }
    }

    // allocate and init audio data
    AudioData data_        = VT_STRUCT(AudioData, 0);
    data_._format          = AUDIO_FILE_FORMAT_WAV;
    data_._channels        = fmt_chunk_._channels;
    data_._sample_rate     = fmt_chunk_._sample_rate;
    data_._bits_per_sample = fmt_chunk_._bits_per_sample;
    data_._channel_format  = (data_._channels == 1 && data_._bits_per_sample == 8)    ? AUDIO_CHANNEL_MONO_8
                             : (data_._channels == 1 && data_._bits_per_sample == 16) ? AUDIO_CHANNEL_MONO_16
                             : (data_._channels == 2 && data_._bits_per_sample == 8)  ? AUDIO_CHANNEL_STEREO_8
                             : (data_._channels == 2 && data_._bits_per_sample == 16) ? AUDIO_CHANNEL_STEREO_16
                                                                                      : -1;

    // seek to the data chunk
    platform_fs_seek_to_position(&handle_, sizeof(AudioWavRiffChunk) + sizeof(AudioWavFmtChunk));

    // read the data chunk
    AudioWavDataChunk data_chunk_;
    while (platform_fs_file_read_data(&handle_, sizeof(data_chunk_._id) + sizeof(data_chunk_._size), NULL,
                                      VT_CAST(VoidPtr, &data_chunk_))) {
        if (misc_str_strncmp(data_chunk_._id, "data", 4, true) == 0) {
            data_chunk_._pcm_data = memory_manager_allocate(0, MEMORY_TAG_AUDIO);
            if (!data_chunk_._pcm_data) {
                VT_LOG_ERROR("Engine", "Failed to allocate memory for PCM data _ filepath: %s", filepath);

                platform_fs_close_file(&handle_);
                return VT_STRUCT(AudioData, 0);
            }

            // seek to the data part to read
            platform_fs_seek_from_current(&handle_, sizeof(data_chunk_._id) + sizeof(data_chunk_._size));

            // read PCM data
            if (!platform_fs_file_read_data(&handle_, data_chunk_._size, NULL, data_chunk_._pcm_data)) {
                VT_LOG_ERROR("Engine", "Failed to read PCM data from WAV file _ filepath: %s", filepath);
                memory_manager_deallocate(data_chunk_._pcm_data, MEMORY_TAG_AUDIO);

                platform_fs_close_file(&handle_);
                return VT_STRUCT(AudioData, 0);
            }

            data_._pcm_data    = data_chunk_._pcm_data;
            data_._data_size   = data_chunk_._size;
            data_._duration_ms = VT_CAST(UInt32, (data_chunk_._size / (data_._channels * (data_._bits_per_sample / 8))) * 1000 /
                                                     data_._sample_rate);
            break;
        } else {
            UInt32 chunk_size_ = VT_STRUCT(UInt32, 0);
            if (!platform_fs_file_read_data(&handle_, sizeof(chunk_size_), NULL, VT_CAST(VoidPtr, &chunk_size_))) {
                VT_LOG_ERROR("Engine", "Failed to read chunk size in WAV file _ filepath: %s", filepath);

                platform_fs_close_file(&handle_);
                return VT_STRUCT(AudioData, 0);
            }

            // skip the chunk
            platform_fs_seek_from_current(&handle_, chunk_size_);
        }
    }

    platform_fs_close_file(&handle_);
    return data_;
}

Bool audio_core_wav_unload_data(AudioData *data) {
    if (data == NULL)
        return false;

    // deallocate PCM data if allocated
    if (data->_pcm_data) {
        memory_manager_deallocate(data->_pcm_data, MEMORY_TAG_AUDIO);
        data->_pcm_data = NULL;
    }

    // set all members to zero
    hal_mem_memzero(data, sizeof(AudioData));

    return true;
}
