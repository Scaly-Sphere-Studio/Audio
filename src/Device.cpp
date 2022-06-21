#include "SSS/Audio/Device.hpp"

SSS_AUDIO_BEGIN;
INTERNAL_BEGIN;

void Device::_init(char const* name)
{
    _device = alcOpenDevice(name);
    if (_device == nullptr) {
        SSS::throw_exc(_internal::getALErrorString(alcGetError(_device)));
    }
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == ALC_TRUE) {
        _current_device = alcGetString(_device, ALC_ALL_DEVICES_SPECIFIER);
    }
    _context = alcCreateContext(_device, nullptr);
    if (_context == nullptr) {
        SSS::throw_exc(_internal::getALErrorString(alcGetError(_device)));
    }
    if (!alcMakeContextCurrent(_context)) {
        SSS::throw_exc(_internal::getALErrorString(alcGetError(_device)));
    }
}

Device::Device() try
{
    updateDevices();
    _init(0);
    LOG_MSG("OpenAL device & context created");
}
CATCH_AND_RETHROW_METHOD_EXC;

Device::~Device()
{
    // Free resources
    cleanSources();
    cleanBuffers();
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
    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == ALC_TRUE) {
        // Retrieve devices
        ALchar const* devices = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
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

Device::List const& Device::getList() const noexcept
{
    return _all_devices;
}

void Device::select(std::string const& name)
{
    bool found = false;
    for (std::string const& listed : _all_devices) {
        if (name == listed) {
            _init(name.c_str());
            found = true;
            break;
        }
    }
    if (!found) {
        LOG_FUNC_CTX_WRN("Couldn't find a device with given name", name);
    }
}

std::string Device::getCurrent() const noexcept
{
    return _current_device;
}

Source::Array const& Device::getSources() const noexcept
{
    return _sources;
}

Buffer::Map const& Device::getBuffers() const noexcept
{
    return _buffers;
}

void Device::cleanSources() noexcept
{
    std::fill(_sources.begin(), _sources.end(), nullptr);
}

void Device::cleanBuffers() noexcept
{
    _buffers.clear();
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

INTERNAL_END;

std::vector<std::string> getDevices() noexcept
{
    try {
        return _internal::Device::get()->getList();
    }
    catch (std::exception const& e) {
        static std::vector<std::string> n;
        LOG_FUNC_ERR(e.what());
        return n;
    }
}

std::string getCurrentDevice() noexcept
{
    try {
        return _internal::Device::get()->getCurrent();
    }
    catch (std::exception const& e) {
        LOG_FUNC_ERR(e.what());
        return nullptr;
    }
}

void selectDevice(std::string const& name) noexcept try
{
    _internal::Device::get()->select(name);
}
CATCH_AND_LOG_FUNC_EXC;

Source::Array const& getSources() noexcept try
{
    return _internal::Device::get()->getSources();
}
catch (std::exception const& e) {
    static Source::Array arr;
    LOG_FUNC_ERR(e.what());
    return arr;
}

Buffer::Map const& getBuffers() noexcept
{
    try {
        return _internal::Device::get()->getBuffers();
    }
    catch (std::exception const& e) {
        static Buffer::Map map;
        LOG_FUNC_ERR(e.what());
        return map;
    }
}

void cleanSources() noexcept try
{
    _internal::Device::get()->cleanSources();
}
CATCH_AND_LOG_FUNC_EXC;

void cleanBuffers() noexcept try
{
    _internal::Device::get()->cleanBuffers();
}
CATCH_AND_LOG_FUNC_EXC;

void createSource(uint32_t id) noexcept try
{
    _internal::Device::get()->createSource(id);
}
CATCH_AND_LOG_FUNC_EXC;

void createBuffer(uint32_t id) noexcept try
{
    _internal::Device::get()->createBuffer(id);
}
CATCH_AND_LOG_FUNC_EXC;

void removeSource(uint32_t id) noexcept try
{
    _internal::Device::get()->removeSource(id);
}
CATCH_AND_LOG_FUNC_EXC;

void removeBuffer(uint32_t id) noexcept try
{
    _internal::Device::get()->removeBuffer(id);
}
CATCH_AND_LOG_FUNC_EXC;

void setMainVolume(int volume) noexcept try
{
    alListenerf(AL_GAIN, static_cast<float>(volume) / 100.f);
}
CATCH_AND_LOG_FUNC_EXC;

int getMainVolume() noexcept
{
    try {
        ALfloat gain;
        alGetListenerf(AL_GAIN, &gain);
        return static_cast<int>(gain * 100.f);
    }
    catch (std::exception const& e) {
        LOG_FUNC_ERR(e.what());
        return 0;
    }
}

SSS_AUDIO_END;