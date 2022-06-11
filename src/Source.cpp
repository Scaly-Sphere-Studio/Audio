#include "SSS/Audio/Source.hpp"
#include "SSS/Audio/Device.hpp"

#define RETURN_IF_NULL if (this == nullptr) return

SSS_AUDIO_BEGIN;

Source::Source()
    : _id([]() {
        ALuint source;
        alGenSources(1, &source);
        if (source == 0) {
            SSS::throw_exc("Couldn't generate an OpenAL source: " + _internal::getALErrorString(alGetError()));
        }
        return source;
    }())
{
}

Source::~Source()
{
    if (_id != 0) {
        stop();
        alSourcei(_id, AL_BUFFER, 0);
        alDeleteSources(1, &_id);
    }
}

ALint Source::_getType() const noexcept
{
    RETURN_IF_NULL 0;
    ALint type;
    alGetSourcei(_id, AL_SOURCE_TYPE, &type);
    return type;
}

void Source::useBuffer(uint32_t id)
{
    RETURN_IF_NULL;
    bool was_playing = false;
    if (!isStopped()) {
        was_playing = isPlaying();
        stop();
    }
    Buffer::Ptr const& buffer = Device::get()->getBuffer(id);
    if (buffer) {
        alSourcei(_id, AL_BUFFER, buffer->_id);
        if (was_playing) {
            play();
        }
    }
}

void Source::queueBuffers(std::vector<uint32_t> ids)
{
    RETURN_IF_NULL;
    // Ensure source has no attached buffer if static
    bool was_playing = false;
    if (_getType() == AL_STATIC) {
        // Stop if needed
        if (!isStopped()) {
            was_playing = isPlaying();
            stop();
        }
        // Detach buffer
        alSourcei(_id, AL_BUFFER, 0);
    }
    std::vector<ALuint> buffer_ids;
    buffer_ids.reserve(ids.size());
    for (uint32_t const& id : ids) {
        Buffer::Ptr const& buffer = Device::get()->getBuffer(id);
        if (buffer) {
            buffer_ids.push_back(buffer->_id);
        }
    }
    if (!buffer_ids.empty()) {
        alSourceQueueBuffers(_id, buffer_ids.size(), &buffer_ids[0]);
        if (was_playing) {
            play();
        }
    }
}

void Source::play()
{
    RETURN_IF_NULL;
    alSourcePlay(_id);
}

void Source::pause()
{
    RETURN_IF_NULL;
    alSourcePause(_id);
}

void Source::stop()
{
    RETURN_IF_NULL;
    alSourceStop(_id);
}

ALint Source::_getState() const noexcept
{
    RETURN_IF_NULL 0;
    ALint state;
    alGetSourcei(_id, AL_SOURCE_STATE, &state);
    return state;
}

bool Source::isPlaying() const noexcept
{
    RETURN_IF_NULL false;
    return _getState() == AL_PLAYING;
}

bool Source::isPaused() const noexcept
{
    RETURN_IF_NULL false;
    return _getState() == AL_PAUSED;
}

bool Source::isStopped() const noexcept
{
    RETURN_IF_NULL false;
    ALint const status = _getState();
    return status == AL_STOPPED || status == AL_INITIAL;
}

ALint Source::getPropertyInt(ALenum param) const
{
    RETURN_IF_NULL 0;
    ALint ret;
    alGetSourcei(_id, param, &ret);
    return ret;;
}

void Source::setPropertyInt(ALenum param, ALint value)
{
    RETURN_IF_NULL;
    alSourcei(_id, param, value);
}

ALfloat Source::getPropertyFloat(ALenum param) const
{
    RETURN_IF_NULL 0.f;
    ALfloat ret;
    alGetSourcef(_id, param, &ret);
    return ret;;
}

void Source::setPropertyFloat(ALenum param, ALfloat value)
{
    RETURN_IF_NULL;
    alSourcef(_id, param, value);
}

SSS_AUDIO_END;