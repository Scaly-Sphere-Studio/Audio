#include "SSS/Audio/Device.hpp"

SSS_AUDIO_BEGIN;
INTERNAL_BEGIN;

void Device::_init(std::string const& name)
{
    char const* id;
    if (_all_devices.count(name) != 0) {
        id = _all_devices.at(name).c_str();
        _current_device = name;
    }
    else {
        id = _all_devices.cbegin()->second.c_str();
        _current_device = _all_devices.cbegin()->first;
    }
    _device = alcOpenDevice(id);
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
}

Device::Device() try
{
    updateDevices();
    _init();
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
        ALCchar const* device = devices, * next = devices + 1;
        // Add each device one by one
        while (device && *device != '\0' && next && *next != '\0') {
            std::string const needle("OpenAL Soft on ");
            std::string key = device;
            size_t const len = key.size();
            size_t const id = key.find(needle);
            if (id + needle.size() < key.size()) {
                key = key.substr(id + needle.size());
            }
            _all_devices[key] = device;
            device += (len + 1);
            next += (len + 2);
        }
    }
}

Device::Map const& Device::getMap() const noexcept
{
    return _all_devices;
}

void Device::select(std::string const& name)
{
    bool found = false;
    for (auto const& pair : _all_devices) {
        if (name == pair.first) {
            _init(pair.first);
            found = true;
            break;
        }
    }
    if (!found) {
        LOG_METHOD_CTX_WRN("Couldn't find a device with given name", name);
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
        std::vector<std::string> vec;
        _internal::Device::Map const& map = _internal::Device::get()->getMap();
        vec.reserve(map.size());
        for (auto const& pair : map) {
            vec.emplace_back(pair.first);
        }
        return vec;
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