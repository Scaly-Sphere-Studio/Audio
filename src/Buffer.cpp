#include "Audio/Buffer.hpp"
#include "Audio/Source.hpp"

SSS_AUDIO_BEGIN;


INTERNAL_BEGIN;
std::string getALErrorString(ALenum error)
{
    switch (error)
    {
    case AL_INVALID_NAME:
        return "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
    case AL_INVALID_ENUM:
        return "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
    case AL_INVALID_VALUE:
        return "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
    case AL_INVALID_OPERATION:
        return "AL_INVALID_OPERATION: the requested operation is not valid";
    case AL_OUT_OF_MEMORY:
        return "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
    default:
        return CONTEXT_MSG("UNKNOWN AL ERROR", error);
    }
}
INTERNAL_END;


std::map<uint32_t, std::unique_ptr<Buffer>> Buffer::_instances{};


Buffer::Buffer(uint32_t id)
    : _openal_id([]() {
        init();
        ALuint buffer;
        alGenBuffers(1, &buffer);
        if (buffer == 0) {
            SSS::throw_exc("Couldn't generate an OpenAL buffer: " + _internal::getALErrorString(alGetError()));
        }
        return buffer;
    }()),
    _map_id(id)
{
}


Buffer::~Buffer()
{
    if (_openal_id != 0) {
        _removeFromSources();
        alDeleteBuffers(1, &_openal_id);
    }
}



Buffer& Buffer::create(uint32_t id)
{
    _instances[id].reset(new Buffer(id));
    return *_instances.at(id);
}


Buffer& Buffer::create()
{
    uint32_t id = 0;
    // Increment ID until no similar value is found
    while (_instances.count(id) != 0) {
        ++id;
    }
    return create(id);
}

Buffer& Buffer::create(std::string const& filename)
{
    auto& buff = create();
    buff.loadFile(filename);
    return buff;
}


Buffer* Buffer::get(uint32_t id) noexcept
{
    if (_instances.count(id) == 0)
        return nullptr;
    return _instances.at(id).get();
}


void Buffer::remove(uint32_t id)
{
    if (_instances.count(id) == 0) {
        _instances.erase(_instances.find(id));
    }
}


void Buffer::loadFile(const std::string& filename) try
{
    // Open audio file
    SF_INFO file_infos;
    SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &file_infos);
    if (file == nullptr) {
        SSS::throw_exc("Couldn't open " + filename);
    }
    // Extract sample size & rate
    ALsizei const sample_nb = static_cast<ALsizei>(file_infos.channels * file_infos.frames);
    ALsizei const sample_hz = static_cast<ALsizei>(file_infos.samplerate);
    // Read by chunks of 16 bits
    std::vector<short> samples(sample_nb);
    sf_count_t const read_nb = sf_readf_short(file, &samples[0], sample_nb);
    if (read_nb < sample_nb) {
        //SSS::throw_exc("Error reading " + filename);
    }
    // Close audio file
    sf_close(file);

    // Determine format
    ALenum format;
    switch (file_infos.channels) {
    case 1:
        format = AL_FORMAT_MONO16;
        break;
    case 2:
        format = AL_FORMAT_STEREO16;
        break;
    default:
        SSS::throw_exc("Unsupported file format");
    }

    // Ensure buffer isn't attached to any source
    _removeFromSources();

    // Fill buffer
    alBufferData(_openal_id, format, &samples[0], sample_nb * sizeof(short), sample_hz);
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        SSS::throw_exc("Error filling buffer: " + _internal::getALErrorString(err));
    }
}
CATCH_AND_LOG_METHOD_EXC;


ALint Buffer::getProperty(ALenum param) const
{
    ALint ret;
    alGetBufferi(_openal_id, param, &ret);
    return ret;;
}



void Buffer::_removeFromSources() noexcept try
{
    for (auto const& source : Source::getArray()) {
        if (source)
            source->_removeBuffer(_openal_id);
    }
}
CATCH_AND_LOG_FUNC_EXC;

SSS_AUDIO_END;