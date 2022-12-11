#pragma once

#ifndef NEALOG_HEADERONLY
#include "nealog/Logger.h"
#endif // !NEALOG_HEADERONLY

#include <array>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <type_traits>


namespace nealog
{

    /******************************
     * LoggerBase
     ******************************/
    // {{{

    NL_INLINE auto LoggerBase::getSinks() -> const std::vector<Sink::SPtr>
    {
        return sinks_;
    }



    NL_INLINE auto LoggerBase::addSink(const Sink::SPtr& sink) -> void
    {
        sinks_.emplace_back(sink);
    }



    NL_INLINE auto LoggerBase::log(Severity messageSeverity, const std::string_view& message) -> void
    {
        if (sinks_.empty())
        {
            parent_->log(messageSeverity, message);
            return;
        }

        if (messageSeverity >= severity_)
        {
            writeToSinks(messageSeverity, message);
        }
    }



    NL_INLINE auto LoggerBase::writeToSinks(Severity severity, const std::string_view& message) -> void
    {
        for (Sink::SPtr& sink : sinks_)
        {
            sink->write(severity, message);
        }
    }



    NL_INLINE auto LoggerBase::trace(const std::string_view& message) -> void
    {
        log(Severity::Trace, message);
    }



    NL_INLINE auto LoggerBase::debug(const std::string_view& message) -> void
    {
        log(Severity::Debug, message);
    }



    NL_INLINE auto LoggerBase::info(const std::string_view& message) -> void
    {
        log(Severity::Info, message);
    }



    NL_INLINE auto LoggerBase::warn(const std::string_view& message) -> void
    {
        log(Severity::Warn, message);
    }



    NL_INLINE auto LoggerBase::error(const std::string_view& message) -> void
    {
        log(Severity::Error, message);
    }



    NL_INLINE auto LoggerBase::fatal(const std::string_view& message) -> void
    {
        log(Severity::Fatal, message);
    }



    NL_INLINE auto LoggerBase::setParent(LoggerBase::SPtr parent) -> void
    {
        parent_ = parent;
    }
    // }}}



    /******************************
     * Logger
     ******************************/
    // {{{

    NL_INLINE Logger::Logger(const std::string& name) noexcept
    {
        name_ = name;
    }    
    // }}}



    NL_INLINE auto ParentHolder::setParentForLogger(LoggerBase::SPtr parent, LoggerBase::SPtr child) -> void
    {
        child->setParent(parent);
    }



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

} // namespace nealog
