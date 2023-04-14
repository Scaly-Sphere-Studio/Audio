#ifndef SSS_AUDIO_LUA_HPP
#define SSS_AUDIO_LUA_HPP

#include <sol/sol.hpp>
#include "_includes.hpp"

SSS_AUDIO_BEGIN;

SSS_AUDIO_API void lua_setup_Audio(sol::state& lua);

SSS_AUDIO_END;

#endif // SSS_AUDIO_LUA_HPP