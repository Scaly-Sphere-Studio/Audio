#pragma once

#include "SSS/Audio/Source.hpp"
#include "SSS/Audio/Buffer.hpp"

SSS_AUDIO_BEGIN;

class Device final {
public:
    using Ptr = std::unique_ptr<Device>;
private:
    Device();
public:
    ~Device();

    static Ptr const& get();

    void updateDevices();

    Source::Ptr const& getSource(uint32_t id) const noexcept;

    void createBuffer(uint32_t id);
    void removeBuffer(uint32_t id);
    Buffer::Ptr const& getBuffer(uint32_t id) const noexcept;

private:
    std::vector<std::string> _all_devices;
    ALCdevice* _device;
    ALCcontext* _context;

    std::array<Source::Ptr, 256> _sources;
    std::map<uint32_t, Buffer::Ptr> _buffers;
};

SSS_AUDIO_END;