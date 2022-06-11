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
    Source(ALuint id) noexcept;
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