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

    NL_INLINE Logger::Logger(std::string_view name) noexcept
    : name_{name}
    {
    }



    NL_INLINE Logger::Logger(std::string_view name, Severity defaultSeverity) noexcept : name_{name}
    {
        setSeverity(defaultSeverity);
    }

    NL_INLINE Logger::Logger(std::string_view name, Severity defaultSeverity,
                             std::vector<Sink::SPtr>&& parentSinks)
        : name_{name}, sinks_{parentSinks}
    {
        setSeverity(defaultSeverity);
    }

    NL_INLINE auto Logger::getSinks() const -> std::vector<Sink::SPtr>
    {
        return sinks_;
    }


    NL_INLINE auto Logger::addSink(const Sink::SPtr& sink) -> void
    {
        sinks_.emplace_back(sink);
    }



    NL_INLINE auto Logger::log(Severity messageSeverity, std::string_view message) -> void
    {
        if (messageSeverity >= severity_)
        {
            std::string formattedMessage = formatter_.format(message);
            writeToSinks(messageSeverity, formattedMessage);
        }
    }



    NL_INLINE auto Logger::writeToSinks(Severity severity, std::string_view message) -> void
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



    NL_INLINE auto Logger::trace(std::string_view message) -> void
    {
        log(Severity::Trace, message);
    }



    NL_INLINE auto Logger::debug(std::string_view message) -> void
    {
        log(Severity::Debug, message);
    }



    NL_INLINE auto Logger::info(std::string_view message) -> void
    {
        log(Severity::Info, message);
    }



    NL_INLINE auto Logger::warn(std::string_view message) -> void
    {
        log(Severity::Warn, message);
    }



    NL_INLINE auto Logger::error(std::string_view message) -> void
    {
        log(Severity::Error, message);
    }



    NL_INLINE auto Logger::fatal(std::string_view message) -> void
    {
        log(Severity::Fatal, message);
    }



    NL_INLINE auto Logger::getName()-> std::string_view const
    {
        return name_;
    }



    // }}}


} // namespace nealog
