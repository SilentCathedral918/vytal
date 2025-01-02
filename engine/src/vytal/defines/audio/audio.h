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

typedef enum Audio_File_Format { AUDIO_FILE_FORMAT_WAV, AUDIO_FILE_FORMAT_OGG, AUDIO_FILE_FORMAT_FLAC } AudioFormat;

typedef enum Audio_Channel_Format {
    AUDIO_CHANNEL_MONO_8,
    AUDIO_CHANNEL_MONO_16,
    AUDIO_CHANNEL_STEREO_8,
    AUDIO_CHANNEL_STEREO_16,
} AudioChannelFormat;

// ---------------------------- playback state ---------------------------- //

typedef enum Audio_Playback_State { AUDIO_PLAYBACK_STOPPED, AUDIO_PLAYBACK_PLAYING, AUDIO_PLAYBACK_PAUSED } AudioPlaybackState;

// ---------------------------- audio backend ---------------------------- //

typedef enum Audio_Backend { AUDIO_BACKEND_OPENAL } AudioBackend;

// ---------------------------- audio data ---------------------------- //

typedef struct Audio_Data {
    UInt64             _id;
    AudioFormat        _format;
    VoidPtr            _pcm_data;
    ByteSize           _data_size;
    UInt16             _channels;
    UInt32             _sample_rate;
    UInt32             _bits_per_sample;
    UInt32             _duration_ms;
    AudioPlaybackState _playback_state;
    UInt32             _playback_position;
    ByteSize           _num_refs;
    AudioChannelFormat _channel_format;
} AudioData;

// ---------------------------- audio source ---------------------------- //

typedef struct Audio_Source {
    UInt32             _id;
    Flt32              _position[3];
    Flt32              _velocity[3];
    Flt32              _direction[6];
    Flt32              _volume;
    Flt32              _pitch;
    Bool               _loop;
    AudioPlaybackState _playback_state;
    Bool               _omnidirectional;
    VoidPtr            _metadata;
} AudioSource;

// ---------------------------- audio buffer ---------------------------- //

typedef struct Audio_Buffer {
    UInt32     _id;
    AudioData *_data;
    VoidPtr    _metadata;
} AudioBuffer;
