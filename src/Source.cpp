#include "SSS/Audio/Source.hpp"
#include "SSS/Audio/Device.hpp"

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
        alSourcei(_id, AL_BUFFER, 0);
        alDeleteSources(1, &_id);
    }
}

void Source::useBuffer(uint32_t id)
{
    Buffer::Ptr const& buffer = Device::get()->getBuffer(id);
    if (buffer) {
        alSourcei(_id, AL_BUFFER, buffer->_id);
    }
}

void Source::play()
{
    alSourcePlay(_id);
}

bool Source::isPlaying() const noexcept
{
    ALint status;
    alGetSourcei(_id, AL_SOURCE_STATE, &status);
    return status == AL_PLAYING;
}

SSS_AUDIO_END;