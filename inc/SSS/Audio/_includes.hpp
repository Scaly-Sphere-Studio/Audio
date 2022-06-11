#pragma once

#include <SSS/Commons.hpp>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <sndfile.h>

#include <map>

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

SSS_AUDIO_BEGIN;
INTERNAL_BEGIN;

std::string getALErrorString(ALenum error);

INTERNAL_END;
SSS_AUDIO_END;

 /** Holds all SSS::Audio related log flags.*/
namespace SSS::Log::Audio {
    LOG_NAMESPACE_BASICS(Log);
}