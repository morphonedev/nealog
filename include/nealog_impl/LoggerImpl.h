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
     * Logger
     ******************************/
    // {{{

    NL_INLINE Logger::Logger(const std::string& name) noexcept
    {
        name_ = name;
    }


    NL_INLINE auto Logger::getSinks() -> const std::vector<Sink::SPtr>
    {
        return sinks_;
    }



    NL_INLINE auto Logger::addSink(const Sink::SPtr& sink) -> void
    {
        sinks_.emplace_back(sink);
    }



    NL_INLINE auto Logger::log(Severity messageSeverity, const std::string_view& message) -> void
    {
        if (sinks_.empty())
        {
            parent_->log(messageSeverity, message);
            return;
        }

        if (messageSeverity >= severity_)
        {
            std::string formattedMessage = formatter_.format(message);
            writeToSinks(messageSeverity, formattedMessage);
        }
    }



    NL_INLINE auto Logger::writeToSinks(Severity severity, const std::string_view& message) -> void
    {
        for (Sink::SPtr& sink : sinks_)
        {
            sink->write(severity, message);
        }
    }



    NL_INLINE auto Logger::setFormatter(const PatternFormatter& formatter) -> void
    {
        formatter_ = formatter;
    }



    NL_INLINE auto Logger::getFormatter() const -> const PatternFormatter&
    {
        return formatter_;
    }



    NL_INLINE auto Logger::trace(const std::string_view& message) -> void
    {
        log(Severity::Trace, message);
    }



    NL_INLINE auto Logger::debug(const std::string_view& message) -> void
    {
        log(Severity::Debug, message);
    }



    NL_INLINE auto Logger::info(const std::string_view& message) -> void
    {
        log(Severity::Info, message);
    }



    NL_INLINE auto Logger::warn(const std::string_view& message) -> void
    {
        log(Severity::Warn, message);
    }



    NL_INLINE auto Logger::error(const std::string_view& message) -> void
    {
        log(Severity::Error, message);
    }



    NL_INLINE auto Logger::fatal(const std::string_view& message) -> void
    {
        log(Severity::Fatal, message);
    }



    NL_INLINE auto Logger::setParent(LoggerBase::SPtr parent) -> void
    {
        parent_ = parent;
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
