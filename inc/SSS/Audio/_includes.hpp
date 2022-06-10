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

inline std::string getALErrorString(ALenum error)
{
    switch (error)
    {
    case AL_INVALID_NAME:
        return "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
    case AL_INVALID_ENUM:
        return "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
    case AL_INVALID_VALUE:
        return "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
    case AL_INVALID_OPERATION:
        return "AL_INVALID_OPERATION: the requested operation is not valid";
    case AL_OUT_OF_MEMORY:
        return "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
    default:
        return CONTEXT_MSG("UNKNOWN AL ERROR", error);
    }
}

INTERNAL_END;
SSS_AUDIO_END;

 /** Holds all SSS::Audio related log flags.*/
namespace SSS::Log::Audio {
    LOG_NAMESPACE_BASICS(Log);
}