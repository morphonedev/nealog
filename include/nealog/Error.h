#pragma once

#include <stdexcept>


namespace nealog
{

    class ParseException : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };


    class LoggerRegistryException : public std::runtime_error
    {
        using std::runtime_error::runtime_error;

      public:
        LoggerRegistryException(const std::string& message);
    };



    class UnregisteredKeyException : public LoggerRegistryException
    {
        using LoggerRegistryException::LoggerRegistryException;

      public:
        UnregisteredKeyException(const std::string& key);

      private:
        const char* key_;
    };
} // namespace nealog


#ifdef NEALOG_HEADERONLY
#include "nealog_impl/ErrorImpl.h"
#endif // NEALOG_HEADERONLY