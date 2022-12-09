#pragma once

#ifndef DROELF_HEADERONLY
#include "nealog/Logger.h"
#endif // !DROELF_HEADERONLY

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

    DLF_INLINE auto LoggerBase::getSinks() -> const std::vector<Sink::SPtr>
    {
        return sinks_;
    }



    DLF_INLINE auto LoggerBase::setParent(LoggerBase::SPtr parent) -> void
    {
        parent_ = parent;
    }
    // }}}



    /******************************
     * Logger
     ******************************/
    // {{{

    DLF_INLINE Logger::Logger(const std::string& name) noexcept
    {
        name_ = name;
    }



    DLF_INLINE auto Logger::addSink(const Sink::SPtr& sink) -> void
    {
        sinks_.emplace_back(sink);
    }



    DLF_INLINE auto Logger::log(Severity messageSeverity, const std::string_view& message) -> void
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



    DLF_INLINE auto Logger::writeToSinks(Severity severity, const std::string_view& message) -> void
    {
        for (Sink::SPtr& sink : sinks_)
        {
            sink->write(severity, message);
        }
    }



    DLF_INLINE auto Logger::trace(const std::string_view& message) -> void
    {
        log(Severity::Trace, message);
    }



    DLF_INLINE auto Logger::debug(const std::string_view& message) -> void
    {
        log(Severity::Debug, message);
    }



    DLF_INLINE auto Logger::info(const std::string_view& message) -> void
    {
        log(Severity::Info, message);
    }



    DLF_INLINE auto Logger::warn(const std::string_view& message) -> void
    {
        log(Severity::Warn, message);
    }



    DLF_INLINE auto Logger::error(const std::string_view& message) -> void
    {
        log(Severity::Error, message);
    }



    DLF_INLINE auto Logger::fatal(const std::string_view& message) -> void
    {
        log(Severity::Fatal, message);
    }

    // }}}



    DLF_INLINE auto ParentHolder::setParentForLogger(LoggerBase::SPtr parent, LoggerBase::SPtr child) -> void
    {
        child->setParent(parent);
    }



    /******************************
     * Exceptions
     ******************************/

    DLF_INLINE LoggerRegistryException::LoggerRegistryException(const std::string& message)
        : std::runtime_error(message){};


#ifndef NO_LOGGER_WITH_KEY_REGISTERED
#define NO_LOGGER_WITH_KEY_REGISTERED(key) "No logger with key \"" + key + "\" registered"
#endif // !NO_LOGGER_WITH_KEY_REGISTERED

    DLF_INLINE UnregisteredKeyException::UnregisteredKeyException(const std::string& key)
        : LoggerRegistryException(NO_LOGGER_WITH_KEY_REGISTERED(key)){};

} // namespace nealog
