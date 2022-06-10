#include "SSS/Audio/Device.hpp"

SSS_AUDIO_BEGIN;

Device::Device() try
{
    updateDevices();
    _device = alcOpenDevice(_all_devices.at(0).c_str());
    if (_device == nullptr) {
        SSS::throw_exc(_internal::getALErrorString(alcGetError(_device)));
    }
    _context = alcCreateContext(_device, nullptr);
    if (_context == nullptr) {
        SSS::throw_exc(_internal::getALErrorString(alcGetError(_device)));
    }
    if (!alcMakeContextCurrent(_context)) {
        SSS::throw_exc(_internal::getALErrorString(alcGetError(_device)));
    }
    LOG_MSG("OpenAL device & context created");
}
CATCH_AND_RETHROW_METHOD_EXC;

Device::~Device()
{
    _sources.clear();
    _buffers.clear();
    // Unbind context
    alcMakeContextCurrent(NULL);
    // Free context & device
    alcDestroyContext(_context);
    alcCloseDevice(_device);
    LOG_MSG("OpenAL device & context destroyed");
}

Device::Ptr const& Device::get()
{
    static Ptr singleton(new Device());
    return singleton;
}

void Device::updateDevices()
{
    _all_devices.clear();
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == ALC_TRUE) {
        // Retrieve devices
        ALchar const* devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        const ALCchar* device = devices, * next = devices + 1;
        size_t len = 0;
        // Add each device one by one
        while (device && *device != '\0' && next && *next != '\0') {
            _all_devices.emplace_back(device);
            len = strlen(device);
            device += (len + 1);
            next += (len + 2);
        }
    }
    if (_all_devices.empty())
        _all_devices.emplace_back(nullptr);
}

void Device::createSource(uint32_t id)
{
    if (_sources.count(id) == 0) {
        _sources.try_emplace(id);
        _sources.at(id).reset(new Source());
    }
}

void Device::removeSource(uint32_t id)
{
    if (_sources.count(id) != 0) {
        _sources.erase(id);
    }
}

Source::Ptr const& Device::getSource(uint32_t id) const noexcept
{
    static Source::Ptr n;
    if (_sources.count(id) != 0) {
        return _sources.at(id);
    }
    return n;
}

void Device::createBuffer(uint32_t id)
{
    if (_buffers.count(id) == 0) {
        _buffers.try_emplace(id);
        _buffers.at(id).reset(new Buffer());
    }
}

void Device::removeBuffer(uint32_t id)
{
    if (_buffers.count(id) != 0) {
        _buffers.erase(id);
    }
}

Buffer::Ptr const& Device::getBuffer(uint32_t id) const noexcept
{
    static Buffer::Ptr n;
    if (_buffers.count(id) != 0) {
        return _buffers.at(id);
    }
    return n;
}

SSS_AUDIO_END;