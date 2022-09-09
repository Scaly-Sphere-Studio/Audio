#define SSS_LUA
#include "SSS/Audio.hpp"

int main() try
{
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::string);
    SSS::Audio::lua_load_audio_functions(lua.globals());
    lua.script_file("Demo.lua");
}
CATCH_AND_LOG_FUNC_EXC;