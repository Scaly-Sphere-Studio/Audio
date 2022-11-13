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
    using Ptr = std::unique_ptr<Buffer>;
    using Map = std::map<uint32_t, Ptr>;

private:
    static Map _instances;
    ALuint const _id;       // OpenAL id
    uint32_t const _map_id; // _instances id
    
    Buffer(uint32_t id);

    void _removeFromSources() noexcept;
public:
    ~Buffer();
    
    static Buffer& create(uint32_t id);
    static Buffer& create();
    static Buffer* get(uint32_t id) noexcept;

    static void remove(uint32_t id);

    inline static Map const& getMap() noexcept { return _instances; };
    inline static void clearAll() noexcept { _instances.clear(); };

    void loadFile(std::string const& filename);

    ALint getProperty(ALenum param) const;
    inline uint32_t getID() const noexcept { return _map_id; };
};

SSS_AUDIO_END;