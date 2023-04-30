#ifndef SSS_AUDIO_SOURCE_HPP
#define SSS_AUDIO_SOURCE_HPP

#include "_includes.hpp"

SSS_AUDIO_BEGIN;

INTERNAL_BEGIN
class Device; // Pre-declaration
INTERNAL_END
class SSS_AUDIO_API Buffer;

// Ignore warning about STL exports as they're private members
#pragma warning(push, 2)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)

class SSS_AUDIO_API Source final : public Base {
    friend _internal::Device;
    friend Buffer;

public:
    Source(const Source&)             = delete; // Copy constructor
    Source(Source&&)                  = delete; // Move constructor
    Source& operator=(const Source&)  = delete; // Copy assignment
    Source& operator=(Source&&)       = delete; // Move assignment
    ~Source();

    static Source& create(uint32_t id);
    static Source& create();
    static Source* get(uint32_t id) noexcept;
    static void remove(uint32_t id);

    inline static auto const& getArray() noexcept { return _instances; };
    static void clearAll() noexcept;

    void useBuffer(uint32_t id);
    void queueBuffers(std::vector<uint32_t> ids);
    void detachBuffers();
    std::vector<uint32_t> getBufferIDs() const noexcept;

    void play();
    void pause();
    void stop();

    bool isPlaying() const noexcept;
    bool isPaused() const noexcept;
    bool isStopped() const noexcept;

    void setVolume(int percentage);
    int getVolume() const;

    void setLooping(bool enable);
    bool isLooping() const;

    ALint getPropertyInt(ALenum param) const;
    void setPropertyInt(ALenum param, ALint value);

    ALfloat getPropertyFloat(ALenum param) const;
    void setPropertyFloat(ALenum param, ALfloat value);

    inline uint32_t getID() const noexcept { return _arr_id; };

private:
    Source(uint32_t id);

    ALint _getType() const noexcept;    // Static, Streaming
    ALint _getState() const noexcept;   // Playing, Paused, Stopped

    // Removes buffer from queue
    void _removeBuffer(ALuint id);

    static std::array<std::unique_ptr<Source>, 256U> _instances;

    ALuint const _openal_id;    // OpenAL id
    uint32_t const _arr_id;     // _instances id

    // OpenAL Buffer ID queue (NOT the ones returned by getBufferIDs)
    std::vector<ALuint> _buffer_ids;
};

#pragma warning(pop)

SSS_AUDIO_END;

#endif // SSS_AUDIO_SOURCE_HPP