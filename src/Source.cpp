#include "SSS/Audio/Source.hpp"
#include "SSS/Audio/Device.hpp"

#define RETURN_IF_NULL if (this == nullptr) return

SSS_AUDIO_BEGIN;


std::array<std::unique_ptr<Source>, 256U> Source::_instances{};


Source::Source(uint32_t id)
    : _openal_id([]() {
        _internal::Device::get();   // Ensure lib is init
        ALuint source;
        alGenSources(1, &source);
        if (source == 0) {
            SSS::throw_exc("Couldn't generate an OpenAL source: " + _internal::getALErrorString(alGetError()));
        }
        return source;
    }()),
    _arr_id(id)
{
}


Source::~Source()
{
    if (_openal_id != 0) {
        stop();
        alSourcei(_openal_id, AL_BUFFER, 0);
        alDeleteSources(1, &_openal_id);
    }
}



Source& Source::create(uint32_t id) try
{
    if (id >= _instances.size()) {
        throw_exc(CONTEXT_MSG("Invalid ID (out of range)", id));
    }
    _instances[id].reset(new Source(id));
    return *_instances.at(id);
}
CATCH_AND_RETHROW_FUNC_EXC;


Source& Source::create() try
{
    uint32_t id = 0;
    // Increment ID until no similar value is used
    while (_instances[id] && id < _instances.size()) {
        ++id;
    }
    if (id >= _instances.size()) {
        SSS::throw_exc("Can't create any OpenAL Audio Source anymore (size full).");
    }
    return create(id);
}
CATCH_AND_RETHROW_FUNC_EXC;


Source* Source::get(uint32_t id) noexcept
{
    return _instances[id].get();
}


void Source::remove(uint32_t id)
{
    _instances[id].reset();
}


void Source::clearAll() noexcept
{
    for (auto& ptr : _instances) {
        ptr.reset();
    }
}


void Source::useBuffer(uint32_t id)
{
    RETURN_IF_NULL;
    Buffer* buffer = Buffer::get(id);
    if (!buffer) {
        LOG_CTX_WRN("SSS/Audio", "Found no Buffer to use at given ID.");
        return;
    }
    bool was_playing = false;
    if (!isStopped()) {
        was_playing = isPlaying();
        stop();
    }
    alSourcei(_openal_id, AL_BUFFER, buffer->_openal_id);
    _buffer_ids.resize(1);
    _buffer_ids[0] = buffer->_openal_id;
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
            alSourcei(_openal_id, AL_BUFFER, 0);
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
        Buffer* buffer = Buffer::get(id);
        if (buffer) {
            openal_ids.push_back(buffer->_openal_id);
            _buffer_ids.push_back(buffer->_openal_id);
        }
    }
    // Queue buffers
    if (!openal_ids.empty()) {
        alSourceQueueBuffers(_openal_id, (ALsizei)openal_ids.size(), &openal_ids[0]);
        // Fast forward played bytes and resume playing if needed
        if (static_bytes_played > 0) {
            setPropertyInt(AL_BYTE_OFFSET, static_bytes_played);
        }
        if (static_was_playing) {
            play();
        }
    }
}


void Source::detachBuffers()
{
    stop();
    alSourcei(_openal_id, AL_BUFFER, 0);
    _buffer_ids.clear();
}


std::vector<uint32_t> Source::getBufferIDs() const noexcept try
{
    std::vector<uint32_t> ids;
    ids.reserve(_buffer_ids.size());
    for (ALuint const& id : _buffer_ids) {
        for (auto const& pair : Buffer::getMap()) {
            if (pair.second->_openal_id == id) {
                ids.push_back(pair.first);
                break;
            }
        }
    }
    return ids;
}
catch (std::exception const& e) {
    LOG_FUNC_ERR(e.what());
    return std::vector<uint32_t>();
}


void Source::play()
{
    RETURN_IF_NULL;
    alSourcePlay(_openal_id);
}


void Source::pause()
{
    RETURN_IF_NULL;
    alSourcePause(_openal_id);
}


void Source::stop()
{
    RETURN_IF_NULL;
    alSourceStop(_openal_id);
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
    alGetSourcei(_openal_id, param, &ret);
    return ret;;
}


void Source::setPropertyInt(ALenum param, ALint value)
{
    RETURN_IF_NULL;
    alSourcei(_openal_id, param, value);
}


ALfloat Source::getPropertyFloat(ALenum param) const
{
    RETURN_IF_NULL 0.f;
    ALfloat ret;
    alGetSourcef(_openal_id, param, &ret);
    return ret;;
}


void Source::setPropertyFloat(ALenum param, ALfloat value)
{
    RETURN_IF_NULL;
    alSourcef(_openal_id, param, value);
}



ALint Source::_getType() const noexcept
{
    RETURN_IF_NULL 0;
    ALint type;
    alGetSourcei(_openal_id, AL_SOURCE_TYPE, &type);
    return type;
}


ALint Source::_getState() const noexcept
{
    RETURN_IF_NULL 0;
    ALint state;
    alGetSourcei(_openal_id, AL_SOURCE_STATE, &state);
    return state;
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
        alSourcei(_openal_id, AL_BUFFER, 0);
        if (!_buffer_ids.empty()) {
            alSourceQueueBuffers(_openal_id, (ALsizei)_buffer_ids.size(), &_buffer_ids[0]);
            if (was_playing) {
                play();
            }
        }
    }
}

SSS_AUDIO_END;