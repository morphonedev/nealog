#pragma once

#include "nealog/Formatter.h"
#include "nealog/Severity.h"
#include "nealog/Sink.h"

namespace nealog
{

    class LoggerBase : public WithSeverity
    {
      public:
        using SPtr = std::shared_ptr<LoggerBase>;

      public:
        virtual ~LoggerBase()               = default;
        LoggerBase()                        = default;
        LoggerBase(LoggerBase&& other)      = default;
        LoggerBase& operator=(LoggerBase&&) = default;

        // make it non-copyable and non-assignable
        LoggerBase(const LoggerBase&)                    = delete;
        LoggerBase(LoggerBase&)                          = delete;
        auto operator=(const LoggerBase&) -> LoggerBase& = delete;
        auto operator=(LoggerBase&) -> LoggerBase&       = delete;

      public:
        virtual auto addSink(const Sink::SPtr&) -> void              = 0;
        virtual auto log(Severity, std::string_view message) -> void = 0;
        virtual auto getSinks() const -> std::vector<Sink::SPtr>     = 0;

        virtual auto setFormatter(const PatternFormatter&) -> void   = 0;
        virtual auto getFormatter() const -> const PatternFormatter& = 0;

        virtual auto trace(std::string_view message) -> void = 0;
        virtual auto debug(std::string_view message) -> void = 0;
        virtual auto info(std::string_view message) -> void  = 0;
        virtual auto warn(std::string_view message) -> void  = 0;
        virtual auto error(std::string_view message) -> void = 0;
        virtual auto fatal(std::string_view message) -> void = 0;

      protected:
        virtual auto writeToSinks(Severity, std::string_view message) -> void = 0;
    };
} // namespace nealog
