#include "SSS/Audio/Buffer.hpp"

SSS_AUDIO_BEGIN;

Buffer::Buffer()
    : _id([]() {
        ALuint buffer;
        alGenBuffers(1, &buffer);
        if (buffer == 0) {
            SSS::throw_exc("Couldn't generate an OpenAL buffer: " + _internal::getALErrorString(alGetError()));
        }
        return buffer;
    }())
{
}

Buffer::~Buffer()
{
    if (_id != 0) {
        alDeleteBuffers(1, &_id);
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
    ALsizei sample_nb = static_cast<ALsizei>(file_infos.channels * file_infos.frames);
    ALsizei sample_rate = static_cast<ALsizei>(file_infos.samplerate);
    // Read by chunks of 16 bits
    std::vector<short> samples(sample_nb);
    if (sf_read_short(file, &samples[0], sample_nb) < sample_nb) {
        SSS::throw_exc("Error reading " + filename);
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

    // Fill buffer
    alBufferData(_id, format, &samples[0], sample_nb * sizeof(short), sample_rate);
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        SSS::throw_exc("Error filling buffer: " + _internal::getALErrorString(err));
    }
}
CATCH_AND_LOG_METHOD_EXC;

SSS_AUDIO_END;