#ifndef SSS_AUDIO_LUA_HPP
#define SSS_AUDIO_LUA_HPP

#define SOL_ALL_SAFETIES_ON 1
#define SOL_STRINGS_ARE_NUMBERS 1
#include <sol/sol.hpp>
#include "Source.hpp"
#include "Buffer.hpp"

SSS_AUDIO_BEGIN;

inline void lua_setup_Audio(sol::state& lua) try
{
    auto audio = lua["Audio"].get_or_create<sol::table>();

    audio["init"] = &init;
    audio["terminate"] = &terminate;

    // Buffer
    auto buffer = audio.new_usertype<Buffer>("Buffer", sol::factories(
        sol::resolve<Buffer& (uint32_t)>(Buffer::create),
        sol::resolve<Buffer& ()>(Buffer::create),
        sol::resolve<Buffer& (std::string const&)>(Buffer::create)),
        sol::base_classes, sol::bases<Base>()
    );
    // Methods
    buffer["loadFile"] = &Buffer::loadFile;
    buffer["getProperty"] = &Buffer::getProperty;
    buffer["id"] = sol::property(&Buffer::getID);
    // Static functions
    buffer["get"] = &Buffer::get;
    buffer["remove"] = &Buffer::remove;
    buffer["clearAll"] = &Buffer::clearAll;

    // Source
    auto source = audio.new_usertype<Source>("Source", sol::factories(
        sol::resolve<Source& (uint32_t)>(Source::create),
        sol::resolve<Source& ()>(Source::create)),
        sol::base_classes, sol::bases<Base>()
    );
    // Settings
    source["useBuffer"] = &Source::useBuffer;
    source["queueBuffers"] = &Source::queueBuffers;
    source["detachBuffers"] = &Source::detachBuffers;
    // Commands
    source["play"] = &Source::play;
    source["pause"] = &Source::pause;
    source["stop"] = &Source::stop;
    // State
    source["is_playing"] = sol::property(&Source::isPlaying);
    source["is_paused"] = sol::property(&Source::isPaused);
    source["is_stopped"] = sol::property(&Source::isStopped);
    // Options
    source["volume"] = sol::property(&Source::getVolume, &Source::setVolume);
    source["loop"] = sol::property(&Source::isLooping, &Source::setLooping);
    source["id"] = sol::property(&Source::getID);
    // Static functions
    source["get"] = &Source::get;
    source["remove"] = &Source::remove;
    source["clearAll"] = &Source::clearAll;

    // Global properties
    audio["getVolume"] = &getMainVolume;
    audio["setVolume"] = &setMainVolume;
    audio["getDevice"] = &getCurrentDevice,
    audio["setDevice"] = &selectDevice;
    audio["getAllDevices"] = &getDevices;
}
CATCH_AND_RETHROW_FUNC_EXC;

SSS_AUDIO_END;

#endif // SSS_AUDIO_LUA_HPP