#pragma once

#include "SSS/Audio/_includes.hpp"

SSS_AUDIO_BEGIN;

INTERNAL_BEGIN
class Device; // Pre-declaration
INTERNAL_END
class Source; // Pre-declaration

class Buffer final {
    friend _internal::Device;
    friend Source;

public:
    Buffer(const Buffer&)             = delete; // Copy constructor
    Buffer(Buffer&&)                  = delete; // Move constructor
    Buffer& operator=(const Buffer&)  = delete; // Copy assignment
    Buffer& operator=(Buffer&&)       = delete; // Move assignment
    ~Buffer();

    static Buffer& create(uint32_t id);
    static Buffer& create();
    static Buffer* get(uint32_t id) noexcept;
    static void remove(uint32_t id);

    inline static auto const& getMap() noexcept { return _instances; };
    inline static void clearAll() noexcept { _instances.clear(); };

    void loadFile(std::string const& filename);

    ALint getProperty(ALenum param) const;
    inline uint32_t getID() const noexcept { return _map_id; };

private:
    Buffer(uint32_t id);

    void _removeFromSources() noexcept;

    static std::map<uint32_t, std::unique_ptr<Buffer>> _instances;

    ALuint const _openal_id;    // OpenAL id
    uint32_t const _map_id;     // _instances id
};

SSS_AUDIO_END;