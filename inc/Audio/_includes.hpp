#ifndef SSS_AUDIO_INCLUDES_HPP
#define SSS_AUDIO_INCLUDES_HPP

#include <SSS/Commons.hpp>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <sndfile.h>

#include <map>
#include <unordered_map>
#include <array>

/** Declares the SSS::Audio namespace.
 *  Further code will be nested in the SSS::Audio namespace.\n
 *  Should be used in pair with with #SSS_AUDIO_END.
 */
#define SSS_AUDIO_BEGIN SSS_BEGIN; namespace Audio {
/** Closes the SSS::Audio namespace declaration.
 *  Further code will no longer be nested in the SSS::Audio namespace.\n
 *  Should be used in pair with with #SSS_AUDIO_BEGIN.
 */
#define SSS_AUDIO_END SSS_END; }

#ifdef SSS_AUDIO_EXPORTS
# define SSS_AUDIO_API __declspec(dllexport)
#else
# ifdef SSS_AUDIO_DEMO
#  define SSS_AUDIO_API
# else
#  define SSS_AUDIO_API __declspec(dllimport)
# endif
#endif

SSS_AUDIO_BEGIN;

INTERNAL_BEGIN;
std::string getALErrorString(ALenum error);
bool is_init() noexcept;
INTERNAL_END;

SSS_AUDIO_API void init();
SSS_AUDIO_API void terminate();

SSS_AUDIO_API std::vector<std::string> getDevices() noexcept;
SSS_AUDIO_API std::string getCurrentDevice() noexcept;
SSS_AUDIO_API void selectDevice(std::string const& name) noexcept;

SSS_AUDIO_API void setMainVolume(int volume) noexcept;
SSS_AUDIO_API int getMainVolume() noexcept;

SSS_AUDIO_END;

 /** Holds all SSS::Audio related log flags.*/
namespace SSS::Log::Audio {
    LOG_NAMESPACE_BASICS(Log);
}

#endif // SSS_AUDIO_INCLUDES_HPP