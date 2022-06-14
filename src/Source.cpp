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

void Source::_removeBuffer(ALuint id)
{
    RETURN_IF_NULL;
    size_t const size_before = _buffer_ids.size();
    _buffer_ids.erase(
        std::remove_if(
            _buffer_ids.begin(),
            _buffer_ids.end(),
            [&](ALuint elem) { return elem == id; }),
        _buffer_ids.end()
    );
    if (_buffer_ids.size() != size_before) {
        bool const was_playing = isPlaying();
        stop();
        if (_getType() == AL_STATIC) {
            alSourcei(_id, AL_BUFFER, 0);
        }
        else {
            bool const was_looping = isLooping();
            setLooping(false);
            std::vector<ALuint> tmp(size_before);
            alSourceUnqueueBuffers(_id, size_before, &tmp[0]);
            setLooping(was_looping);
            if (!_buffer_ids.empty()) {
                alSourceQueueBuffers(_id, _buffer_ids.size(), &_buffer_ids[0]);
                if (was_playing) {
                    play();
                }
            }
        }
    }
}

void Source::useBuffer(uint32_t id)
{
    RETURN_IF_NULL;
    Buffer::Map const& buffers = getBuffers();
    if (buffers.count(id) == 0) {
        LOG_CTX_WRN("SSS/Audio", "Found no Buffer to use at given ID.");
        return;
    }
    Buffer::Ptr const& buffer = getBuffers().at(id);
    if (!buffer) {
        LOG_CTX_WRN("SSS/Audio", "Tried to use non-initialized Buffer.");
        return;
    }
    bool was_playing = false;
    if (!isStopped()) {
        was_playing = isPlaying();
        stop();
    }
    alSourcei(_id, AL_BUFFER, buffer->_id);
    _buffer_ids.resize(1);
    _buffer_ids[0] = buffer->_id;
    if (was_playing) {
        play();
    }
}

void Source::queueBuffers(std::vector<uint32_t> ids)
{
    RETURN_IF_NULL;
    // OpenAL IDs (to be filled)
    std::vector<ALuint> openal_ids;
    openal_ids.reserve(ids.size() + 1);

    // Ensure source has no attached buffer if static
    bool static_was_playing = false; // Static source was playing
    int static_bytes_played = 0;     // Static source played X bytes
    if (_getType() == AL_STATIC) {

        // Stop if needed
        ALint const state = _getState();
        if (state == AL_PLAYING || state == AL_PAUSED) {
            static_was_playing = state == AL_PLAYING;
            static_bytes_played = getPropertyInt(AL_BYTE_OFFSET);
            stop();
        }
        
        _buffer_ids.clear();
        // Check if a buffer was attached
        ALint const current_id = getPropertyInt(AL_BUFFER);
        if (current_id != 0) {
            // Detach buffer
            alSourcei(_id, AL_BUFFER, 0);
            // Add to queue
            openal_ids.push_back(current_id);
            _buffer_ids.push_back(current_id);
            // Fast forward if already played
            if (state == AL_STOPPED) {
                alGetBufferi(current_id, AL_SIZE, &static_bytes_played);
            }
        }
    }
    // Retrieve OpenAL ids
    for (uint32_t const& id : ids) {
        Buffer::Ptr const& buffer = getBuffers().at(id);
        if (buffer) {
            openal_ids.push_back(buffer->_id);
            _buffer_ids.push_back(buffer->_id);
        }
    }
    // Queue buffers
    if (!openal_ids.empty()) {
        alSourceQueueBuffers(_id, openal_ids.size(), &openal_ids[0]);
        // Fast forward played bytes and resume playing if needed
        if (static_bytes_played > 0) {
            setPropertyInt(AL_BYTE_OFFSET, static_bytes_played);
        }
        if (static_was_playing) {
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

void Source::setVolume(int percentage)
{
    setPropertyFloat(AL_GAIN, static_cast<float>(percentage) / 100.f);
}

int Source::getVolume() const
{
    return static_cast<int>(getPropertyFloat(AL_GAIN) * 100.f);
}

void Source::setLooping(bool enable)
{
    setPropertyInt(AL_LOOPING, static_cast<int>(enable));
}

bool Source::isLooping() const
{
    return static_cast<bool>(getPropertyInt(AL_LOOPING));
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