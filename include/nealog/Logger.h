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

    constexpr const char* ROOT_LOGGER_NAME = "";


    class Logger : public LoggerBase
    {
        using LoggerBase::LoggerBase;

      public:
        Logger(const std::string& name) noexcept;

      public:
        auto addSink(const Sink::SPtr&) -> void override;
        auto log(Severity, const std::string_view& message) -> void override;

        auto trace(const std::string_view& message) -> void override;
        auto debug(const std::string_view& message) -> void override;
        auto info(const std::string_view& message) -> void override;
        auto warn(const std::string_view& message) -> void override;
        auto error(const std::string_view& message) -> void override;
        auto fatal(const std::string_view& message) -> void override;

      private:
        auto setParent() -> void;
        auto writeToSinks(Severity, const std::string_view& message) -> void;
    };



    class LoggerRegistryException : public std::runtime_error
    {
        using std::runtime_error::runtime_error;

      public:
        LoggerRegistryException(const std::string& message);
    };



    class UnregisteredKeyException : public LoggerRegistryException
    {
        using LoggerRegistryException::LoggerRegistryException;

      public:
        UnregisteredKeyException(const std::string& key);

      private:
        const char* key_;
    };



    class ParentHolder
    {
      protected:
        auto setParentForLogger(LoggerBase::SPtr parent, LoggerBase::SPtr child) -> void;
    };



    /******************************
     * LoggerRegistry
     ******************************/
    // {{{
    template <class TMutex = MutexWrapper>
    class LoggerRegistry : public ParentHolder
    {
      public:
        const char* LOGGER_TREE_SEPARATOR = ".";

      public:
        auto getOrCreate(const std::string& name) -> LoggerBase::SPtr;

      private:
        auto createLogger(const std::string& name) -> LoggerBase::SPtr;
        auto getParentName(const std::string& name) -> const std::string;

      private:
        TMutex mutex_;
        std::unordered_map<std::string, LoggerBase::SPtr> registrees_{};
    };



    using LoggerRegistry_st = LoggerRegistry<FakeMutex>;
    using LoggerRegistry_mt = LoggerRegistry<RealMutex>;



    template <class TMutex>
    auto LoggerRegistry<TMutex>::getOrCreate(const std::string& name) -> LoggerBase::SPtr
    {
        mutex_.lock();

        LoggerBase::SPtr logger;

        // search in registered loggers
        if (auto it = registrees_.find(name); it != registrees_.end())
            logger = it->second;
        else
            logger = createLogger(name);


        mutex_.unlock();

        return logger;
    }



    template <class TMutex>
    auto LoggerRegistry<TMutex>::createLogger(const std::string& name) -> LoggerBase::SPtr
    {
        auto logger = std::make_shared<Logger>(name);
        if (name != ROOT_LOGGER_NAME)
        {
            auto& parentName = getParentName(name);
            auto parent      = getOrCreate(parentName);
            setParentForLogger(parent, logger);
        }

        registrees_.insert({name, logger});

        return logger;
    }



    template <class TMutex>
    auto LoggerRegistry<TMutex>::getParentName(const std::string& name) -> const std::string
    {
        std::string::size_type index = name.find_last_of(LOGGER_TREE_SEPARATOR, name.length());

        if (index == name.npos)
            return ROOT_LOGGER_NAME;
        else
            return name.substr(0, index);
    }
    //}}}

} // namespace nealog

#ifdef NEALOG_HEADERONLY
#include "nealog_impl/LoggerImpl.h"
#endif // NEALOG_HEADERONLY
