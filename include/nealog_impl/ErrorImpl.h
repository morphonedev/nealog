#pragma once

#ifndef NEALOG_HEADERONLY
#include "nealog/Error.h"
#endif // !NEALOG_HEADERONLY

#include <stdexcept>

namespace nealog
{

    /******************************
     * Exceptions
     ******************************/

    NL_INLINE LoggerRegistryException::LoggerRegistryException(const std::string& message)
        : std::runtime_error(message){};


#ifndef NO_LOGGER_WITH_KEY_REGISTERED
#define NO_LOGGER_WITH_KEY_REGISTERED(key) "No logger with key \"" + key + "\" registered"
#endif // !NO_LOGGER_WITH_KEY_REGISTERED

    NL_INLINE UnregisteredKeyException::UnregisteredKeyException(const std::string& key)
        : LoggerRegistryException(NO_LOGGER_WITH_KEY_REGISTERED(key)){};

}
