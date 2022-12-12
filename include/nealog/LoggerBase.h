#pragma once

#include "nealog/Severity.h"
#include "nealog/Sink.h"

namespace nealog
{

    class LoggerBase : public WithSeverity
    {
        friend class ParentHolder;

      public:
        using SPtr = std::shared_ptr<LoggerBase>;

      public:
        virtual ~LoggerBase() = default;
        LoggerBase()          = default;

      private:
        LoggerBase(const LoggerBase&) = default;
        LoggerBase(LoggerBase&&)      = default;

        auto operator=(const LoggerBase&) -> LoggerBase& = default;
        auto operator=(LoggerBase&&) -> LoggerBase&      = default;

      public:
        virtual auto addSink(const Sink::SPtr&) -> void                     = 0;
        virtual auto log(Severity, const std::string_view& message) -> void = 0;
        virtual auto getSinks() -> const std::vector<Sink::SPtr>            = 0;
        virtual auto trace(const std::string_view& message) -> void         = 0;
        virtual auto debug(const std::string_view& message) -> void         = 0;
        virtual auto info(const std::string_view& message) -> void          = 0;
        virtual auto warn(const std::string_view& message) -> void          = 0;
        virtual auto error(const std::string_view& message) -> void         = 0;
        virtual auto fatal(const std::string_view& message) -> void         = 0;

      protected:
        virtual auto writeToSinks(Severity, const std::string_view& message) -> void = 0;
        virtual auto setParent(LoggerBase::SPtr parent) -> void                      = 0;
    };
} // namespace nealog
