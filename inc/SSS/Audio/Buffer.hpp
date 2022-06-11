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
    ALuint const _id;
    
    Buffer();
public:
    ~Buffer();
    
    void loadFile(std::string const& filename);

    ALint getProperty(ALenum param) const;
};

SSS_AUDIO_END;