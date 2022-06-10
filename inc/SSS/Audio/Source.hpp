#pragma once

#include "SSS/Audio/_includes.hpp"

SSS_AUDIO_BEGIN;

class Device; // Pre-declaration

class Source final {
    friend Device;

public:
    using Ptr = std::unique_ptr<Source>;

private:
    ALuint const _id;

    Source();
public:
    ~Source();

    void useBuffer(uint32_t id);

    void play();
    bool isPlaying() const noexcept;
};

SSS_AUDIO_END;