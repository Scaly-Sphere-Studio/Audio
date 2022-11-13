#pragma once

#include "SSS/Audio/_includes.hpp"

SSS_AUDIO_BEGIN;

INTERNAL_BEGIN
class Device; // Pre-declaration
INTERNAL_END
class Buffer;

class Source final {
    friend _internal::Device;
    friend Buffer;

public:
    using Ptr = std::unique_ptr<Source>;
    using Array = std::array<Ptr, 256U>;

private:
    static Array _instances;
    ALuint const _id;       // OpenAL id
    uint32_t const _arr_id; // _instances id

    Source(uint32_t id);
public:
    ~Source();

    static Source& create(uint32_t id);
    static Source& create();
    static Source* get(uint32_t id) noexcept;

    static void remove(uint32_t id);

    inline static Array const& getArray() noexcept { return _instances; };
    static void clearAll() noexcept;

private:
    ALint _getType() const noexcept;
    std::vector<ALuint> _buffer_ids;
    void _removeBuffer(ALuint id);
public:
    void useBuffer(uint32_t id);
    void queueBuffers(std::vector<uint32_t> ids);
    void detachBuffers();
    std::vector<uint32_t> getBufferIDs() const noexcept;

    void play();
    void pause();
    void stop();

private:
    ALint _getState() const noexcept;
public:
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
};

SSS_AUDIO_END;