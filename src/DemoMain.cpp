#define SSS_LUA
#include "Audio.hpp"

int main() try
{
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::string);
    SSS::Audio::lua_setup_Audio(lua);
    SSS::Audio::init();
    lua.script_file("Demo.lua");
    SSS::Audio::terminate();
}
CATCH_AND_LOG_FUNC_EXC;