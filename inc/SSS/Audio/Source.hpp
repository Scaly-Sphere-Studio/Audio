#pragma once

#include "SSS/Audio/_includes.hpp"

SSS_AUDIO_BEGIN;

INTERNAL_BEGIN
class Device; // Pre-declaration
INTERNAL_END

class Source final {
    friend _internal::Device;

public:
    using Ptr = std::unique_ptr<Source>;
    using Array = std::array<Ptr, 256U>;

private:
    ALuint const _id;

    Source();
public:
    ~Source();

private:
    ALint _getType() const noexcept;
public:
    void useBuffer(uint32_t id);
    void queueBuffers(std::vector<uint32_t> ids);

    void play();
    void pause();
    void stop();

private:
    ALint _getState() const noexcept;
public:
    bool isPlaying() const noexcept;
    bool isPaused() const noexcept;
    bool isStopped() const noexcept;

    ALint getPropertyInt(ALenum param) const;
    void setPropertyInt(ALenum param, ALint value);

    ALfloat getPropertyFloat(ALenum param) const;
    void setPropertyFloat(ALenum param, ALfloat value);
};

SSS_AUDIO_END;