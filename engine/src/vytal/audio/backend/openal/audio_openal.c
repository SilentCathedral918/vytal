#include "audio_openal.h"

#include "vytal/core/hal/memory/vtmem.h"

#include <AL/al.h>
#include <AL/alc.h>

// --------------------------------- device --------------------------------- //

AudioDevice audio_backend_al_open_device(ConstStr device_name) {
    return VT_STRUCT(AudioDevice, ._handle = alcOpenDevice(device_name));
}

Bool audio_backend_al_close_device(AudioDevice *device) {
    if (!device)
        return false;

    return alcCloseDevice(device->_handle);
}

Bool audio_backend_al_get_context_device(AudioDevice *out_device, const AudioContext *context) {
    if (!out_device || !context)
        return false;

    out_device->_handle = alcGetContextsDevice(context->_handle);
    return true;
}

// --------------------------------- context --------------------------------- //

AudioContext audio_backend_al_create_context(const AudioDevice *device, const Int32 *attr_list) {
    if (!device)
        return VT_STRUCT(AudioContext, 0);

    return VT_STRUCT(AudioContext, ._handle = alcCreateContext(device->_handle, attr_list));
}

Bool audio_backend_al_destroy_context(AudioContext *context) {
    if (!context)
        return false;

    alcDestroyContext(context->_handle);
    return true;
}

Bool audio_backend_al_suspend_context(AudioContext *context) {
    if (!context)
        return false;

    alcSuspendContext(context->_handle);
    return true;
}

Bool audio_backend_al_make_context_current(VoidPtr context) { return alcMakeContextCurrent(context); }

Bool audio_backend_al_process_context(AudioContext *context) {
    if (!context)
        return false;

    alcProcessContext(context->_handle);
    return true;
}

Bool audio_backend_al_get_current_context(AudioContext *out_context) {
    if (!out_context)
        return false;

    out_context->_handle = alcGetCurrentContext();
    return true;
}

// --------------------------------- listener --------------------------------- //

Bool audio_backend_al_set_listener_position(AudioListener *listener, const Flt32 x, const Flt32 y, const Flt32 z) {
    if (!listener)
        return false;

    listener->_position[0] = x;
    listener->_position[1] = y;
    listener->_position[2] = z;
    alListener3f(AL_POSITION, x, y, z);

    return true;
}

Bool audio_backend_al_set_listener_velocity(AudioListener *listener, const Flt32 x, const Flt32 y, const Flt32 z) {
    if (!listener)
        return false;

    listener->_velocity[0] = x;
    listener->_velocity[1] = y;
    listener->_velocity[2] = z;
    alListener3f(AL_VELOCITY, x, y, z);

    return true;
}

Bool audio_backend_al_set_listener_orientation(AudioListener *listener, Flt32 *orientation) {
    if (!listener)
        return false;

    hal_mem_memcpy(listener->_orientation, orientation, sizeof(listener->_orientation));
    alListenerfv(AL_ORIENTATION, orientation);

    return true;
}

// --------------------------------- source --------------------------------- //

Bool audio_backend_al_generate_source(UInt32 *source) {
    if (!source)
        return false;

    alGenSources(1, source);
    return true;
}

Bool audio_backend_al_generate_sources(UInt32 *sources, const ByteSize count) {
    if (!sources || (count == 0))
        return false;

    alGenSources(count, sources);
    return true;
}

Bool audio_backend_al_delete_source(UInt32 *source) {
    if (!source)
        return false;

    alDeleteSources(1, source);
    return true;
}

Bool audio_backend_al_delete_sources(UInt32 *sources, const ByteSize count) {
    if (!sources || (count == 0))
        return false;

    alDeleteSources(count, sources);
    return true;
}

void audio_backend_al_attach_buffer_to_source(UInt32 source, const UInt32 buffer) { alSourcei(source, AL_BUFFER, buffer); }

void audio_backend_al_source_play(const UInt32 source) { alSourcePlay(source); }

AudioPlaybackState audio_backend_al_get_playback_state(const UInt32 source) {
    ALint state_;
    alGetSourcei(source, AL_SOURCE_STATE, &state_);

    switch (state_) {
    case AL_STOPPED:
        return AUDIO_PLAYBACK_STOPPED;

    case AL_PLAYING:
        return AUDIO_PLAYBACK_PLAYING;

    case AL_PAUSED:
        return AUDIO_PLAYBACK_PAUSED;

    default:
        return AL_INVALID;
    }
}

// --------------------------------- buffer --------------------------------- //

Bool audio_backend_al_generate_buffer(UInt32 *buffer) {
    if (!buffer)
        return false;

    alGenBuffers(1, buffer);
    return true;
}

Bool audio_backend_al_generate_buffers(UInt32 *buffers, const ByteSize count) {
    if (!buffers || (count == 0))
        return false;

    alGenBuffers(count, buffers);
    return true;
}

Bool audio_backend_al_delete_buffer(UInt32 *buffer) {
    if (!buffer)
        return false;

    alDeleteBuffers(1, buffer);
    return true;
}

Bool audio_backend_al_delete_buffers(UInt32 *buffers, const ByteSize count) {
    if (!buffers || (count == 0))
        return false;

    alDeleteBuffers(count, buffers);
    return true;
}

Bool audio_backend_al_buffer_fill_data(const UInt32 buffer, const AudioChannelFormat channel_format, const VoidPtr data,
                                       const ByteSize size, const ByteSize sample_rate) {
    if ((buffer == 0x00) || !data || (size == 0) || (sample_rate == 0))
        return false;

    ALenum channel_format_ = 0;
    switch (channel_format) {
    case AUDIO_CHANNEL_MONO_8:
        channel_format_ = AL_FORMAT_MONO8;
        break;

    case AUDIO_CHANNEL_MONO_16:
        channel_format_ = AL_FORMAT_MONO16;
        break;

    case AUDIO_CHANNEL_STEREO_8:
        channel_format_ = AL_FORMAT_STEREO8;
        break;

    case AUDIO_CHANNEL_STEREO_16:
        channel_format_ = AL_FORMAT_STEREO16;
        break;

    default:
        return false;
    }

    alBufferData(buffer, channel_format_, data, size, sample_rate);
    return true;
}

Bool audio_backend_al_is_buffer_valid(const UInt32 buffer) {
    if (buffer == 0x00)
        return false;

    return alIsBuffer(buffer);
}