#pragma once

#include "vytal/defines/core/types.h"

// ---------------------------- audio device ---------------------------- //

typedef struct Audio_Device {
    VoidPtr _handle;
} AudioDevice;

// ---------------------------- audio context ---------------------------- //

typedef struct Audio_Context {
    VoidPtr _handle;
} AudioContext;

// ---------------------------- audio listener ---------------------------- //

typedef struct Audio_Listener {
    Flt32 _position[3];
    Flt32 _orientation[6];
    Flt32 _velocity[3];
} AudioListener;

// ---------------------------- audio formats ---------------------------- //

typedef enum Audio_Format { AUDIO_FORMAT_WAV, AUDIO_FORMAT_OGG, AUDIO_FORMAT_FLAC } AudioFormat;

// ---------------------------- playback state ---------------------------- //

typedef enum Audio_Playback_State { AUDIO_PLAYBACK_STOPPED, AUDIO_PLAYBACK_PLAYING, AUDIO_PLAYBACK_PAUSED } AudioPlaybackState;

// ---------------------------- audio backend ---------------------------- //

typedef enum Audio_Backend { AUDIO_BACKEND_OPENAL } AudioBackend;

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
