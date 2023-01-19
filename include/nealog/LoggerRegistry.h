#pragma once

#include <string_view>
#include <unordered_map>

#include "Error.h"
#include "Logger.h"
#include "Severity.h"
#include "Utility.h"

namespace nealog
{

    template <class LoggerClass, class SinkClass, class SeverityClass>
    class LoggerTreeRegistry
    {
      public:
        using LoggerStorage = std::unordered_map<std::string_view, LoggerClass>;


        LoggerTreeRegistry(unsigned char delim = '.')
            : delimiter_{delim}, rootLogger_{rootLoggerName, SeverityClass::Trace}
        {
        }
        auto getOrCreate(std::string_view name) -> LoggerClass&;
        auto getOrCreate(void) -> LoggerClass&;
        auto getLoggerList()->const typename LoggerStorage&;
        auto setBranchSeverity(std::string_view branchRoot, SeverityClass newLevel) -> void;
        auto setBranchSeverity(SeverityClass newLevel) -> void;
        auto addBranchSink(const typename SinkClass::SPtr& sink) -> void;
        auto addBranchSink(std::string_view branchRoot, const typename SinkClass::SPtr& sink) -> void;

      private:
        auto registerLogger(std::string_view name) -> LoggerClass&;

      private:
        static constexpr std::string_view rootLoggerName = "root";
        unsigned char delimiter_;
        LoggerStorage loggerTree_;
        LoggerClass rootLogger_;
    };

    using LoggerTreeRegistry_st = LoggerTreeRegistry<Logger, Sink, Severity>;

    /******************************
     * LoggerTreeRegistry
     ******************************/
    // {{{

    template <class LoggerClass, class SinkClass, class SeverityClass>
    auto LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::getOrCreate(void) -> LoggerClass&
    {
        return rootLogger_;
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    auto  LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::getOrCreate(std::string_view name) -> LoggerClass&
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


    template <class LoggerClass, class SinkClass, class SeverityClass>
    auto LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::getLoggerList() -> const
        typename LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::LoggerStorage&
    {
        return loggerTree_;
    }



    /********************** Private methods *******************************/
    template <class LoggerClass, class SinkClass, class SeverityClass>
    auto LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::registerLogger(std::string_view name)-> LoggerClass&
    {
        std::cout << "Registering logger " << name << std::endl;
        auto result = name.rfind(delimiter_);
        if (result != std::string_view::npos) // found at least one delimiter
        {
            LoggerClass& parentLogger = getOrCreate(name.substr(0, result));
            // inherit from parentLogger
            auto result = loggerTree_.emplace(std::make_pair<std::string_view, LoggerClass>(
                std::string_view(name), LoggerClass{name, parentLogger.getSeverity(), parentLogger.getSinks()}));
            return (result.first->second);
        }
        else // inherit from root logger
        {
            auto& parentLogger = rootLogger_;
            auto result        = loggerTree_.emplace(std::make_pair<std::string_view, LoggerClass>(
                std::string_view(name), LoggerClass(name, parentLogger.getSeverity(), parentLogger.getSinks())));
            return (result.first->second);
        }
        return rootLogger_;
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    auto LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::setBranchSeverity(std::string_view branchRoot,
                                                                                      SeverityClass newLevel) -> void
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
            if (utility::beginswith(name, branchRoot))
            {
                std::cout << name << " is child of " << branchRoot << std::endl;
                logger.setSeverity(newLevel);
            }
        }
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    auto LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::setBranchSeverity(SeverityClass newLevel) -> void
    {
        rootLogger_.setSeverity(newLevel);
        for (auto& [name, logger] : loggerTree_)
        {
            logger.setSeverity(newLevel);
        }
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    auto LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::addBranchSink(const typename SinkClass::SPtr& sink) -> void
    {
        rootLogger_.addSink(sink);
        for (auto& [name, logger] : loggerTree_)
        {
            logger.addSink(sink);
        }
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    auto LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::addBranchSink(std::string_view branchRoot,
                                                                                  const typename SinkClass::SPtr& sink) -> void
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
            if (utility::beginswith(name, branchRoot))
            {
                std::cout << name << " is child of " << branchRoot << std::endl;
                logger.addSink(sink);
            }
        }
    }

    // }}}



} // namespace nealog