#pragma once

#include "defines/core/types.h"

// ---------------------------- audio formats ---------------------------- //

typedef enum Audio_Format { AUDIO_FORMAT_WAV, AUDIO_FORMAT_OGG, AUDIO_FORMAT_FLAC } AudioFormat;

// ---------------------------- playback state ---------------------------- //

typedef enum Audio_Playback_State { AUDIO_PLAYBACK_STOPPED, AUDIO_PLAYBACK_PLAYING, AUDIO_PLAYBACK_PAUSED } AudioPlaybackState;

// ---------------------------- audio data ---------------------------- //

typedef struct Audio_Data {
    Int64              _id;
    AudioFormat        _format;
    VoidPtr            _pcm_data;
    ByteSize           _data_size;
    UInt16             _channels;
    UInt32             _sample_rate;
    UInt32             _bits_per_sample;
    UInt32             _duration_ms;
    Bool               _stream;
    Bool               _loop;
    AudioPlaybackState _playback_state;
    UInt32             _playback_position;
} AudioData;
