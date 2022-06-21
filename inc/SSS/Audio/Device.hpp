#pragma once

#include "SSS/Audio/Source.hpp"
#include "SSS/Audio/Buffer.hpp"

SSS_AUDIO_BEGIN;

Source::Array const& getSources() noexcept;
Buffer::Map const& getBuffers() noexcept;
void cleanSources() noexcept;
void cleanBuffers() noexcept;
void createSource(uint32_t id) noexcept;
void createBuffer(uint32_t id) noexcept;
void removeSource(uint32_t id) noexcept;
void removeBuffer(uint32_t id) noexcept;

void setMainVolume(int volume) noexcept;
int getMainVolume() noexcept;

INTERNAL_BEGIN;
class Device final {
private:
    Device();
public:
    ~Device();
    // Unique ptr type, returned by get()
    using Ptr = std::unique_ptr<Device>;
    // Returns singleton
    static Ptr const& get();

private:
    // All devices listed by OpenAL
    std::vector<std::string> _all_devices;
    // Current OpenAL device
    ALCdevice* _device;
    // Current OpenAL context
    ALCcontext* _context;

    // Fixed array of Sources
    Source::Array _sources;
    // ID Map of Buffers
    Buffer::Map _buffers;

public:
    // Updates _all_devices
    void updateDevices();

    Source::Array const& getSources() const noexcept;
    Buffer::Map const& getBuffers() const noexcept;

    void cleanSources() noexcept;
    void cleanBuffers() noexcept;

    // Creates and stores a Source in _sources at given ID
    void createSource(uint32_t id);
    // Removes the Source in _sources at given ID
    void removeSource(uint32_t id);

    // Creates and stores a Buffer in _buffers at given ID
    void createBuffer(uint32_t id);
    // Removes the Buffer in _buffers at given ID
    void removeBuffer(uint32_t id);
};
INTERNAL_END;

SSS_AUDIO_END;