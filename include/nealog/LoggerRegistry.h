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

    template <class Mutex>
    class LoggerTreeRegistry
    {
      public:
        using LoggerStorage = std::unordered_map<std::string_view, Logger>;


        LoggerTreeRegistry(unsigned char delim = '.', std::string rootLoggerName = "root")
            : rootLoggerName_{rootLoggerName}, delimiter_{delim}, rootLogger_{rootLoggerName_, Severity::Trace}
        {
        }
        auto getOrCreate(std::string_view name) -> Logger&;
        auto getRootLogger(void) -> Logger&;
        auto getLoggerList() -> const typename LoggerStorage&;
        auto setBranchSeverity(std::string_view branchRoot, Severity newLevel) -> void;
        auto setTreeSeverity(Severity newLevel) -> void;
        auto addTreeSink(const Sink::SPtr& sink) -> void;
        auto addBranchSink(std::string_view branchRoot, const Sink::SPtr& sink) -> void;

      private:
        auto registerLogger(std::string_view name) -> Logger&;

      private:
        const std::string rootLoggerName_;
        unsigned char delimiter_;
        LoggerStorage loggerTree_;
        Logger rootLogger_;
        mutable Mutex mutex_;
    };


    using LoggerTreeRegistry_st = LoggerTreeRegistry<FakeMutex>;
    using LoggerTreeRegistry_mt = LoggerTreeRegistry<RealMutex>;


    /******************************
     * LoggerTreeRegistry
     ******************************/
    // {{{

    template <class Mutex>
    auto LoggerTreeRegistry<Mutex>::getRootLogger(void) -> Logger&
    {
        return rootLogger_;
    }



    template <class Mutex>
    auto LoggerTreeRegistry<Mutex>::getOrCreate(std::string_view name) -> Logger&
    {
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


    template <class Mutex>
    auto LoggerTreeRegistry<Mutex>::getLoggerList() -> const LoggerTreeRegistry<Mutex>::LoggerStorage&
    {
        return loggerTree_;
    }



    /********************** Private methods *******************************/
    template <class Mutex>
    auto LoggerTreeRegistry<Mutex>::registerLogger(std::string_view name) -> Logger&
    {
        auto result = name.rfind(delimiter_);
        if (result != std::string_view::npos) // found at least one delimiter
        {
            Logger& parentLogger = getOrCreate(name.substr(0, result));
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



    template <class Mutex>
    auto LoggerTreeRegistry<Mutex>::setBranchSeverity(std::string_view branchRoot, Severity newLevel) -> void
    {
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



    template <class Mutex>
    auto LoggerTreeRegistry<Mutex>::setTreeSeverity(Severity newLevel) -> void
    {
        rootLogger_.setSeverity(newLevel);
        for (auto& [name, logger] : loggerTree_)
        {
            logger.setSeverity(newLevel);
        }
    }



    template <class Mutex>
    auto LoggerTreeRegistry<Mutex>::addTreeSink(const Sink::SPtr& sink) -> void
    {
        rootLogger_.addSink(sink);
        for (auto& [name, logger] : loggerTree_)
        {
            logger.addSink(sink);
        }
    }



    template <class Mutex>
    auto LoggerTreeRegistry<Mutex>::addBranchSink(std::string_view branchRoot, const Sink::SPtr& sink) -> void
    {
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

    // }}}



} // namespace nealog