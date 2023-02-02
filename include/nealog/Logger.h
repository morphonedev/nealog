#pragma once

#include "nealog/LoggerBase.h"
#include "nealog/Mutex.h"
#include "nealog/Severity.h"
#include "nealog/Sink.h"

#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace nealog
{

    class Logger : public LoggerBase
    {
        using LoggerBase::LoggerBase;

      public:
        Logger(std::string_view name) noexcept;
        Logger(std::string_view name, Severity defaultSeverity) noexcept;
        Logger(std::string_view name, Severity defaultSeverity, std::vector<Sink::SPtr>&& parentSinks);

      public:
        auto addSink(const Sink::SPtr&) -> void override;
        auto log(Severity, std::string_view message) -> void override;
        auto getSinks() const -> std::vector<Sink::SPtr> override;
        auto setFormatter(const PatternFormatter&) -> void override;
        auto getFormatter() const -> const PatternFormatter& override;
        auto getName() -> std::string_view const;

        auto trace(std::string_view message) -> void override;
        auto debug(std::string_view message) -> void override;
        auto info(std::string_view message) -> void override;
        auto warn(std::string_view message) -> void override;
        auto error(std::string_view message) -> void override;
        auto fatal(std::string_view message) -> void override;

      protected:
        auto writeToSinks(Severity, std::string_view message) -> void override;

      protected:
        std::vector<Sink::SPtr> sinks_{};
        std::string_view name_{};
        PatternFormatter formatter_{""};
    };


} // namespace nealog

#ifdef NEALOG_HEADERONLY
#include "nealog_impl/LoggerImpl.h"
#endif // NEALOG_HEADERONLY
