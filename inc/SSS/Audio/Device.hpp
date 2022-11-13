#pragma once

#include "SSS/Audio/Source.hpp"
#include "SSS/Audio/Buffer.hpp"

SSS_AUDIO_BEGIN;
INTERNAL_BEGIN;
class Device final {
private:
    void _init(std::string const& name = "");
    Device();
public:
    ~Device();
    // Unique ptr type, returned by get()
    using Ptr = std::unique_ptr<Device>;
    // Returns singleton
    static Ptr const& get();

    using Map = std::unordered_map<std::string, std::string>;
private:
    // All devices listed by OpenAL
    Map _all_devices;
    // Current device name
    std::string _current_device;
    // Current OpenAL device
    ALCdevice* _device;
    // Current OpenAL context
    ALCcontext* _context;

public:
    // Updates _all_devices
    void updateDevices();
    Map const& getMap() const noexcept;
    void select(std::string const& name);
    std::string getCurrent() const noexcept;

    void setMainVolume(int volume) noexcept;
    int getMainVolume() const noexcept;
};
INTERNAL_END;

std::vector<std::string> getDevices() noexcept;
std::string getCurrentDevice() noexcept;
void selectDevice(std::string const& name) noexcept;

void setMainVolume(int volume) noexcept;
int getMainVolume() noexcept;


SSS_AUDIO_END;