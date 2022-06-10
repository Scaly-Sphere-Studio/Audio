#pragma once

#include "SSS/Audio/_includes.hpp"

SSS_AUDIO_BEGIN;

class Device; class Source; // Pre-declaration

class Buffer final {
    friend Device;
    friend Source;

public:
    using Ptr = std::unique_ptr<Buffer>;

private:
    ALuint const _id;
    
    Buffer();
public:
    ~Buffer();
    
    void loadFile(std::string const& filename);
};

SSS_AUDIO_END;