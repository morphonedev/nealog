#pragma once

#include <string_view>
#include <unordered_map>

#include "Logger.h"
#include "Severity.h"

namespace nealog
{

    template <class LoggerClass, class SinkClass, class SeverityClass>
    class LoggerTreeRegistry
    {

      public:
        LoggerTreeRegistry() : delimiter_{'.'}, rootLogger_{LoggerClass(SeverityClass::Trace, rootLoggerName)}
        {
        }
        LoggerClass& getOrCreate(std::string_view name);
        LoggerClass& getOrCreate(void);
        void setDelimiter(unsigned char delim);
        const std::unordered_map<std::string_view, LoggerClass>& getLoggerList();
        void setBranchSeverity(std::string_view branchRoot, SeverityClass newLevel);
        void setBranchSeverity(SeverityClass newLevel);
        void addBranchSink(typename SinkClass::UPtr&& sink);
        void addBranchSink(std::string_view branchRoot, typename SinkClass::UPtr&& sink);

      private:
        LoggerClass& registerLogger(std::string_view name);

      private:
        static constexpr std::string_view rootLoggerName = "root";
        unsigned char delimiter_;
        std::unordered_map<std::string_view, LoggerClass> loggerTree_;
        LoggerClass rootLogger_;
    };

    using LoggerTreeRegistry_st = LoggerTreeRegistry<Logger, Sink, Severity>;

    /******************************
     * LoggerTreeRegistry
     ******************************/
    // {{{

    template <class LoggerClass, class SinkClass, class SeverityClass>
    LoggerClass& LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::getOrCreate(void)
    {
        return rootLogger_;
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    inline LoggerClass& LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::getOrCreate(std::string_view name)
    {
        std::cout << "Trying to find logger with name: " << name << std::endl;
        auto result = loggerTree_.find(name);
        if (result != std::end(loggerTree_))
        {
            std::cout << "Found logger with name " << name << std::endl;
            return result->second;
        }
        else
        {
            return registerLogger(name);
        }
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    inline void LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::setDelimiter(unsigned char delim)
    {
        delimiter_ = delim;
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    const std::unordered_map<std::string_view, LoggerClass>& LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::getLoggerList()
    {
        return loggerTree_;
    }



    /********************** Private methods *******************************/
    template <class LoggerClass, class SinkClass, class SeverityClass>
    inline LoggerClass& LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::registerLogger(std::string_view name)
    {
        std::cout << "Registering logger " << name << std::endl;
        auto result = name.rfind(delimiter_);
        if (result != std::string_view::npos) // found at least one delimiter
        {
            auto& parentLogger = getLogger(name.substr(0, result));
            // inherit from parentLogger
            auto result = loggerTree_.emplace(std::make_pair<std::string_view, LoggerClass>(
                std::string_view(name),
                Logger(parentLogger.getLevel(), name, parentLogger.getLinkedSinks(), parentLogger.getOwnedSinks())));
            std::cout << parentLogger.toString() << "'s child logger " << result.first->second.toString()
                      << " was inserted: " << result.second << std::endl;
            return (result.first->second);
        }
        else // inherit from root logger
        {
            auto& parentLogger = rootLogger_;
            auto result        = loggerTree_.emplace(std::make_pair<std::string_view, LoggerClass>(
                std::string_view(name),
                Logger(parentLogger.getLevel(), name, parentLogger.getLinkedSinks(), parentLogger.getOwnedSinks())));
            std::cout << parentLogger.toString() << "'s child logger " << result.first->second.toString()
                      << " was inserted: " << result.second << std::endl;
            return (result.first->second);
        }
        return rootLogger_;
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    void LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::setBranchSeverity(std::string_view branchRoot, SeverityClass newLevel)
    {
        auto result = loggerTree_.find(branchRoot);
        if (result != std::end(loggerTree_))
        {
            result->second.setLevel(newLevel);
        }
        for (auto& elem : loggerTree_)
        {
            if (bzlog::utility::beginswith(elem.first, branchRoot))
            {
                std::cout << elem.first << " is child of " << branchRoot << std::endl;
                elem.second.setLevel(newLevel);
            }
        }
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    void LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::setBranchSeverity(SeverityClass newLevel)
    {
        rootLogger_.setLevel(newLevel);
        for (auto& elem : loggerTree_)
        {
            elem.second.setLevel(newLevel);
        }
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    void LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::addBranchSink(typename SinkClass::UPtr&& sink)
    {
        auto newsink = rootLogger_.addSink(std::move(sink));
        for (auto& elem : loggerTree_)
        {
            elem.second.addLinkedSink(newsink);
        }
    }



    template <class LoggerClass, class SinkClass, class SeverityClass>
    void LoggerTreeRegistry<LoggerClass, SinkClass, SeverityClass>::addBranchSink(std::string_view branchRoot, typename SinkClass::UPtr&& sink)
    {
    }

    // }}}



} // namespace nealog