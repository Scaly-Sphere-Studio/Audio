#pragma once

#include "SSS/Audio/Source.hpp"
#include "SSS/Audio/Buffer.hpp"

SSS_AUDIO_BEGIN;

INTERNAL_BEGIN;
class Device final {
public:
    Device(const Device&)             = delete; // Copy constructor
    Device(Device&&)                  = delete; // Move constructor
    Device& operator=(const Device&)  = delete; // Copy assignment
    Device& operator=(Device&&)       = delete; // Move assignment
    ~Device();

    // Returns singleton
    static std::unique_ptr<Device> const& get();

    // Updates _all_devices
    void updateDevices();
    inline auto const& getAllDevices() const noexcept { return _all_devices; };
    inline std::string getCurrentDevice() const noexcept { return _current_device; };
    void selectDevice(std::string const& name);

    void setMainVolume(int volume) noexcept;
    int getMainVolume() const noexcept;

private:
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
INTERNAL_END;

std::vector<std::string> getDevices() noexcept;
std::string getCurrentDevice() noexcept;
void selectDevice(std::string const& name) noexcept;

void setMainVolume(int volume) noexcept;
int getMainVolume() noexcept;


SSS_AUDIO_END;