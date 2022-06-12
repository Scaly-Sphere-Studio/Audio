#include "SSS/Audio/Device.hpp"

SSS_AUDIO_BEGIN;
INTERNAL_BEGIN;

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
    std::fill(_sources.begin(), _sources.end(), nullptr);
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

void Device::createSource(uint32_t id)
{
    if (id >= _sources.size()) {
        SSS::throw_exc("Can't create more than 256 sources.");
    }
    if (!_sources.at(id)) {
        _sources.at(id).reset(new Source());
    }
}

void Device::removeSource(uint32_t id)
{
    if (_sources.at(id)) {
        _sources.at(id).reset();
    }
}

INTERNAL_END;

void createBuffer(uint32_t id) noexcept try
{
    _internal::Device::get()->createBuffer(id);
}
CATCH_AND_LOG_FUNC_EXC;

void createSource(uint32_t id) noexcept try
{
    _internal::Device::get()->createSource(id);
}
CATCH_AND_LOG_FUNC_EXC;

void removeBuffer(uint32_t id) noexcept try
{
    _internal::Device::get()->removeBuffer(id);
}
CATCH_AND_LOG_FUNC_EXC;

void removeSource(uint32_t id) noexcept try
{
    _internal::Device::get()->removeSource(id);
}
CATCH_AND_LOG_FUNC_EXC;

Buffer::Map const& getBuffers() noexcept
{
    return _internal::Device::get()->_buffers;
}

Source::Array const& getSources() noexcept
{
    return _internal::Device::get()->_sources;
}

SSS_AUDIO_END;