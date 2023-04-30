#include "Audio/Source.hpp"
#include "Audio/Buffer.hpp"

SSS_AUDIO_BEGIN;
INTERNAL_BEGIN;

class Device final {
    friend void ::SSS::Audio::init();
    friend void ::SSS::Audio::terminate();
    friend bool is_init() noexcept;
public:
    Device(const Device&) = delete; // Copy constructor
    Device(Device&&) = delete; // Move constructor
    Device& operator=(const Device&) = delete; // Copy assignment
    Device& operator=(Device&&) = delete; // Move assignment
    ~Device();

    // Returns singleton
    static Device& get();

    // Updates _all_devices
    void updateDevices();
    inline auto const& getAllDevices() const noexcept { return _all_devices; };
    inline std::string getCurrentDevice() const noexcept { return _current_device; };
    void selectDevice(std::string const& name);

    void setMainVolume(int volume) noexcept;
    int getMainVolume() const noexcept;

private:
    static std::unique_ptr<Device> _ptr;
    Device();

    void _init(std::string const& name = "");

    // All devices listed by OpenAL
    std::unordered_map<std::string, std::string> _all_devices;
    // Current device name
    std::string _current_device;
    // Current OpenAL device
    ALCdevice* _device;
    // Current OpenAL context
    ALCcontext* _context;
};

std::unique_ptr<Device> Device::_ptr{};

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
    Source::clearAll();
    Buffer::clearAll();
    // Unbind context
    alcMakeContextCurrent(NULL);
    // Free context & device
    alcDestroyContext(_context);
    alcCloseDevice(_device);
    LOG_MSG("OpenAL device & context destroyed");
}

Device& Device::get()
{
    init();
    return *_ptr;
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


void Device::selectDevice(std::string const& name)
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


void Device::setMainVolume(int volume) noexcept try
{
    alListenerf(AL_GAIN, static_cast<float>(volume) / 100.f);
}
CATCH_AND_LOG_FUNC_EXC;


int Device::getMainVolume() const noexcept
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

bool is_init() noexcept
{
    return !!Device::_ptr;
}

INTERNAL_END;

void init()
{
    if (!_internal::Device::_ptr)
        _internal::Device::_ptr.reset(new _internal::Device);
}

void terminate()
{
    _internal::Device::_ptr.reset();
}


std::vector<std::string> getDevices() noexcept
{
    try {
        std::vector<std::string> vec;
        auto const& map = _internal::Device::get().getAllDevices();
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
        return _internal::Device::get().getCurrentDevice();
    }
    catch (std::exception const& e) {
        LOG_FUNC_ERR(e.what());
        return nullptr;
    }
}


void selectDevice(std::string const& name) noexcept try
{
    _internal::Device::get().selectDevice(name);
}
CATCH_AND_LOG_FUNC_EXC;


void setMainVolume(int volume) noexcept try
{
    _internal::Device::get().setMainVolume(volume);
}
CATCH_AND_LOG_FUNC_EXC;


int getMainVolume() noexcept
{
    return _internal::Device::get().getMainVolume();
}

SSS_AUDIO_END;