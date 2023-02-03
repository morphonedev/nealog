#pragma once

#include <string_view>
#include <unordered_map>

#include "Error.h"
#include "Logger.h"
#include "Mutex.h"
#include "Severity.h"
#include "Utility.h"

namespace nealog
{

    template <class LoggerClass, class Mutex>
    class LoggerTreeRegistry
    {
      public:
        using LoggerStorage = std::unordered_map<std::string_view, LoggerClass>;


        LoggerTreeRegistry(unsigned char delim = '.', std::string rootLoggerName = "root")
            : rootLoggerName_{rootLoggerName}, delimiter_{delim}, rootLogger_{rootLoggerName_, Severity::Trace}
        {
        }
        auto getOrCreate(std::string_view name) -> LoggerClass&;
        auto getRootLogger(void) -> LoggerClass&;
        auto getLoggerList() -> const LoggerStorage&;
        auto setBranchSeverity(std::string_view branchRoot, Severity newLevel) -> void;
        auto setTreeSeverity(Severity newLevel) -> void;
        auto addTreeSink(const Sink::SPtr& sink) -> void;
        auto addBranchSink(std::string_view branchRoot, const Sink::SPtr& sink) -> void;

      private:
        auto registerLogger(std::string_view name) -> LoggerClass&;

      private:
        const std::string rootLoggerName_;
        unsigned char delimiter_;
        LoggerStorage loggerTree_;
        Logger rootLogger_;
        mutable Mutex mutex_;
    };


    using LoggerTreeRegistry_st = LoggerTreeRegistry<Logger, FakeMutex>;
    using LoggerTreeRegistry_mt = LoggerTreeRegistry<Logger, RealMutex>;


    /******************************
     * LoggerTreeRegistry
     ******************************/
    // {{{

    template <class LoggerClass, class Mutex>
    auto LoggerTreeRegistry<LoggerClass, Mutex>::getRootLogger(void) -> LoggerClass&
    {
        return rootLogger_;
    }



    template <class LoggerClass, class Mutex>
    auto LoggerTreeRegistry<LoggerClass, Mutex>::getOrCreate(std::string_view name) -> LoggerClass&
    {
        std::scoped_lock<Mutex> lock{mutex_};
        auto result = loggerTree_.find(name);

        if (result != std::end(loggerTree_))
        {
            return result->second;
        }
        else
        {
            return registerLogger(name);
        }
    }


    template <class LoggerClass, class Mutex>
    auto LoggerTreeRegistry<LoggerClass, Mutex>::getLoggerList() -> const LoggerTreeRegistry<LoggerClass, Mutex>::LoggerStorage&
    {
        std::scoped_lock<Mutex> lock{mutex_};
        return loggerTree_;
    }



    template <class LoggerClass, class Mutex>
    auto LoggerTreeRegistry<LoggerClass, Mutex>::setBranchSeverity(std::string_view branchRoot, Severity newLevel) -> void
    {
        std::scoped_lock<Mutex> lock{mutex_};
        auto result = loggerTree_.find(branchRoot);
        if (result != std::end(loggerTree_))
        {
            result->second.setSeverity(newLevel);
        }
        else
        {
            throw UnregisteredKeyException(std::string{branchRoot});
        }
        for (auto& [name, logger] : loggerTree_)
        {
            if (utility::beginsWith(name, branchRoot))
            {
                logger.setSeverity(newLevel);
            }
        }
    }



    template <class LoggerClass, class Mutex>
    auto LoggerTreeRegistry<LoggerClass, Mutex>::setTreeSeverity(Severity newLevel) -> void
    {
        std::scoped_lock<Mutex> lock{mutex_};
        rootLogger_.setSeverity(newLevel);
        for (auto& [name, logger] : loggerTree_)
        {
            logger.setSeverity(newLevel);
        }
    }



    template <class LoggerClass, class Mutex>
    auto LoggerTreeRegistry<LoggerClass, Mutex>::addTreeSink(const Sink::SPtr& sink) -> void
    {
        std::scoped_lock<Mutex> lock{mutex_};
        rootLogger_.addSink(sink);
        for (auto& [name, logger] : loggerTree_)
        {
            logger.addSink(sink);
        }
    }



    template <class LoggerClass, class Mutex>
    auto LoggerTreeRegistry<LoggerClass, Mutex>::addBranchSink(std::string_view branchRoot, const Sink::SPtr& sink) -> void
    {
        std::scoped_lock<Mutex> lock{mutex_};
        auto result = loggerTree_.find(branchRoot);
        if (result != std::end(loggerTree_))
        {
            result->second.addSink(sink);
        }
        else
        {
            throw UnregisteredKeyException(std::string{branchRoot});
        }
        for (auto& [name, logger] : loggerTree_)
        {
            if (utility::beginsWith(name, branchRoot))
            {
                logger.addSink(sink);
            }
        }
    }



    /********************** Private methods *******************************/
    template <class LoggerClass, class Mutex>
    auto LoggerTreeRegistry<LoggerClass, Mutex>::registerLogger(std::string_view name) -> LoggerClass&
    {
        auto result = name.rfind(delimiter_);
        if (result != std::string_view::npos) // found at least one delimiter
        {
            LoggerClass& parentLogger = getOrCreate(name.substr(0, result));
            // inherit from parentLogger
            auto result = loggerTree_.emplace(std::make_pair<std::string_view, Logger>(
                std::string_view(name), Logger{name, parentLogger.getSeverity(), parentLogger.getSinks()}));
            return (result.first->second);
        }
        else // inherit from root logger
        {
            auto& parentLogger = rootLogger_;
            auto result        = loggerTree_.emplace(std::make_pair<std::string_view, Logger>(
                std::string_view(name), Logger(name, parentLogger.getSeverity(), parentLogger.getSinks())));
            return (result.first->second);
        }
        return rootLogger_;
    }



    // }}}



} // namespace nealog