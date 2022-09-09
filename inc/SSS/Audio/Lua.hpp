#pragma once

#include "SSS/Audio/Device.hpp"

#ifdef SSS_LUA
#include <sol/sol.hpp>

SSS_AUDIO_BEGIN;

template <bool b>
inline void lua_load_audio_functions(sol::table_core<b>& lua) try
{
    auto audio = lua["Audio"].get_or_create<sol::table>();

    // Buffer
    audio.new_usertype<Buffer>(
        "Buffer",
        // Methods
        "loadFile", &Buffer::loadFile,
        "getProperty", &Buffer::getProperty,
        // Static functions
        "get", ([](uint32_t id) {
            auto const& buffers = getBuffers();
            if (buffers.count(id) == 0) createBuffer(id);
            return buffers.at(id);
        }),
        "clean", &cleanBuffers,
        "create", &createBuffer,
        "remove", &removeBuffer
    );

    // Source
    audio.new_usertype<Source>(
        "Source",
        // Settings
        "useBuffer", &Source::useBuffer,
        "queueBuffers", &Source::queueBuffers,
        "detachBuffers", &Source::detachBuffers,
        // Commands
        "play", &Source::play,
        "pause", &Source::pause,
        "stop", &Source::stop,
        // State
        "is_playing", sol::property(&Source::isPlaying),
        "is_paused", sol::property(&Source::isPaused),
        "is_stopped", sol::property(&Source::isStopped),
        // Options
        "volume", sol::property(&Source::getVolume, &Source::setVolume),
        "loop", sol::property(&Source::isLooping, &Source::setLooping),
        // Static functions
        "get", ([](uint32_t id) {
            auto const& source = getSources().at(id);
            if (!source) createSource(id);
            return source;
        }),
        "clean", &cleanSources,
        "create", &createSource,
        "remove", &removeSource
    );

    // Global properties
    audio["getVolume"] = &getMainVolume;
    audio["setVolume"] = &setMainVolume;
    audio["getDevice"] = &getCurrentDevice,
    audio["setDevice"] = &selectDevice;
    audio["getAllDevices"] = &getDevices;
}
CATCH_AND_RETHROW_FUNC_EXC;

SSS_AUDIO_END;
#endif