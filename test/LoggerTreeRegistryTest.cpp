#include "TestApi.h"
#include "nealog/Error.h"
#include "nealog/Logger.h"
#include "nealog/LoggerRegistry.h"
#include "nealog/Sink.h"

#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <array>

using namespace std::chrono_literals;
using namespace nealog;

constexpr const char* TAG             = "[LoggerTreeRegistry]";
constexpr const char* TAG_INTEGRATION = "[LoggerTreeRegistry][Integration]";
constexpr const char* TAG_THREADING   = "[LoggerTreeRegistry][Multithreading]";


TEST_CASE("should create new logger", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    requireResultEqualsExpected(rootLogger.getName(), "root");
    auto& logger = registry.getOrCreate("com");
    requireResultNotEqualsExpected(&rootLogger, &logger);
}



TEST_CASE("should get existing logger", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& logger       = registry.getOrCreate("com");
    auto& secondLogger = registry.getOrCreate("com");
    requireResultEqualsExpected(&secondLogger, &logger);
}



TEST_CASE("Add intermediate loggers if necessary", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& childLogger = registry.getOrCreate("child.subchild.somelogger");

    auto& childLoggers = registry.getLoggerList();

    std::vector<std::string> loggerNames{"child", "child.subchild", "child.subchild.somelogger"};

    for (auto& loggerName : loggerNames)
    {
        auto result = childLoggers.find(loggerName);
        requireResultNotEqualsExpected(result, childLoggers.end());
    }
}



TEST_CASE("Inherit sink from root logger", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    rootLogger.addSink(SinkFactory::createStdOutSink());
    auto rootSinks    = rootLogger.getSinks();
    auto& childLogger = registry.getOrCreate("child");
    auto childSinks   = childLogger.getSinks();
    requireResultEqualsExpected(rootSinks.size(), childSinks.size());
    for (auto& sink : rootSinks)
    {
        bool found{false};
        for (auto& csink : childSinks)
        {
            if (csink.get() == sink.get())
            {
                found = true;
                break;
            }
        }
        requireResultEqualsExpected(found, true);
    }
}

TEST_CASE("Inherit sink from root logger subtree", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    auto mainSink    = SinkFactory::createStdOutSink();
    rootLogger.addSink(mainSink);
    auto rootSinks     = rootLogger.getSinks();
    auto& childLogger  = registry.getOrCreate("child.subchild.somelogger");
    auto& childLoggers = registry.getLoggerList();
    for (auto& child_logger : childLoggers)
    {
        auto childSinks = childLogger.getSinks();
        requireResultEqualsExpected(rootSinks.size(), childSinks.size());
        bool found{false};
        for (auto& csink : childSinks)
        {
            if (csink.get() == mainSink.get())
            {
                found = true;
                break;
            }
        }
        requireResultEqualsExpected(found, true);
    }
}



TEST_CASE("child misses sink added after creation", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    rootLogger.addSink(SinkFactory::createStdOutSink());

    auto& childLogger = registry.getOrCreate("child");
    auto streamSink   = SinkFactory::createStdOutSink();
    rootLogger.addSink(streamSink); // Add a second sink to root logger

    auto rootSinks  = rootLogger.getSinks();
    auto childSinks = childLogger.getSinks();

    bool found{false};
    // second sink in root logger sinks
    for (auto& sink : rootSinks)
    {
        if (sink.get() == streamSink.get())
        {
            found = true;
            break;
        }
    }
    requireResultEqualsExpected(found, true);

    found = false;
    // second sink NOT in child logger sinks
    for (auto& sink : childSinks)
    {
        if (sink.get() == streamSink.get())
        {
            found = true;
            break;
        }
    }
    requireResultEqualsExpected(found, false);
}



TEST_CASE("Inherit severity from root logger", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    rootLogger.setSeverity(nealog::Severity::Warn);
    auto rootSeverity  = rootLogger.getSeverity();
    auto& childLogger  = registry.getOrCreate("child");
    auto childSeverity = childLogger.getSeverity();
    requireResultEqualsExpected(childSeverity, rootSeverity);
}



TEST_CASE("Inherit severity from root logger subtree", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    auto mainSink    = SinkFactory::createStdOutSink();
    rootLogger.setSeverity(nealog::Severity::Error);
    auto rootSeverity = rootLogger.getSeverity();
    auto& childLogger = registry.getOrCreate("child.subchild.somelogger");
    std::vector<std::string> loggerNames{"child", "child.subchild", "child.subchild.somelogger"};

    for (auto& loggerName : loggerNames)
    {
        auto childSeverity = registry.getOrCreate(loggerName).getSeverity();
        requireResultEqualsExpected(rootSeverity, childSeverity);
    }
}


TEST_CASE("child misses severity change after creation", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    rootLogger.addSink(SinkFactory::createStdOutSink());
    rootLogger.setSeverity(nealog::Severity::Error);
    auto& childLogger = registry.getOrCreate("child");
    requireResultEqualsExpected(rootLogger.getSeverity(), nealog::Severity::Error);
    requireResultEqualsExpected(childLogger.getSeverity(), nealog::Severity::Error);
    rootLogger.setSeverity(nealog::Severity::Info);
    requireResultEqualsExpected(rootLogger.getSeverity(), nealog::Severity::Info);
    requireResultEqualsExpected(childLogger.getSeverity(), nealog::Severity::Error);
}



TEST_CASE("Recursivly adding sink adds it to all children of root logger", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    auto mainSink    = SinkFactory::createStdOutSink();

    auto& childLogger   = registry.getOrCreate("child.subchild.somelogger");
    auto& siblingLogger = registry.getOrCreate("sibling.subsibling.somelogger");
    registry.addTreeSink(mainSink);
    auto rootSinks    = rootLogger.getSinks();
    auto& childLoggers = registry.getLoggerList();
    for (auto& child_logger : childLoggers)
    {
        auto childSinks = childLogger.getSinks();
        requireResultEqualsExpected(rootSinks.size(), childSinks.size());
        bool found{false};
        for (auto& csink : childSinks)
        {
            if (csink.get() == mainSink.get())
            {
                found = true;
                break;
            }
        }
        requireResultEqualsExpected(found, true);
    }
}

TEST_CASE("Recursivly adding sink adds it to all children of subtree logger", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    auto mainSink    = SinkFactory::createStdOutSink();

    auto& childLogger   = registry.getOrCreate("child.subchild.somelogger");
    auto& siblingLogger = registry.getOrCreate("sibling.subsibling.somelogger");
    registry.addBranchSink("child.subchild", mainSink);
    auto subtreeSinks = registry.getOrCreate("child.subchild").getSinks();
    auto& childLoggers = registry.getLoggerList();
    for (auto& child_logger : childLoggers)
    {
        bool found{false};
        auto childSinks = child_logger.second.getSinks();
        for (auto& csink : childSinks)
        {
            if (csink.get() == mainSink.get())
            {
                found = true;
                break;
            }
        }
        if ((child_logger.first == "child.subchild") || (child_logger.first == "child.subchild.somelogger"))
        {

            requireResultEqualsExpected(found, true);
        }
        else
        {
            requireResultEqualsExpected(found, false);
        }
    }
}

TEST_CASE("Recursivly adding sink to non exisintg subtree throws exception", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    auto mainSink    = SinkFactory::createStdOutSink();

    auto& childLogger   = registry.getOrCreate("child.subchild.somelogger");
    auto& siblingLogger = registry.getOrCreate("sibling.subsibling.somelogger");
    REQUIRE_THROWS_AS(registry.addBranchSink("subchild", mainSink), nealog::UnregisteredKeyException);
}

TEST_CASE("Recursivly changing severity, changes severity of all children of root logger", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    rootLogger.setSeverity(nealog::Severity::Fatal);
    requireResultEqualsExpected(rootLogger.getSeverity(), nealog::Severity::Fatal);
    auto& childLogger   = registry.getOrCreate("child.subchild.somelogger");
    auto& siblingLogger = registry.getOrCreate("sibling.subsibling.somelogger");
    registry.setTreeSeverity(nealog::Severity::Trace);
    auto rootSeverity  = rootLogger.getSeverity();
    auto& childLoggers = registry.getLoggerList();
    for (auto& child_logger : childLoggers)
    {
        auto childServerity = childLogger.getSeverity();
        requireResultEqualsExpected(childServerity, rootSeverity);
    }
}


TEST_CASE("Recursivly changing severity of subtree, only changes severity of all children of subtree", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    rootLogger.setSeverity(nealog::Severity::Fatal);
    requireResultEqualsExpected(rootLogger.getSeverity(), nealog::Severity::Fatal);
    auto& childLogger   = registry.getOrCreate("child.subchild.somelogger");
    auto& siblingLogger = registry.getOrCreate("sibling.subsibling.somelogger");
    registry.setBranchSeverity("sibling.subsibling", nealog::Severity::Trace);
    auto rootSeverity = rootLogger.getSeverity();
    requireResultEqualsExpected(nealog::Severity::Fatal, rootSeverity);
    auto& childLoggers = registry.getLoggerList();
    for (const auto& child_logger : childLoggers)
    {
        if ((child_logger.first == "sibling.subsibling") || (child_logger.first == "sibling.subsibling.somelogger"))
        {
            auto childSeverity = child_logger.second.getSeverity();
            requireResultEqualsExpected(childSeverity, nealog::Severity::Trace);
        }
        else
        {
            requireResultEqualsExpected(child_logger.second.getSeverity(), rootSeverity);
        }
    }
}

TEST_CASE("Recursivly changing severity of non exisintg subtree throws exception", TAG)
{
    LoggerTreeRegistry_st registry;
    auto& rootLogger = registry.getRootLogger();
    auto mainSink    = SinkFactory::createStdOutSink();

    auto& childLogger   = registry.getOrCreate("child.subchild.somelogger");
    auto& siblingLogger = registry.getOrCreate("sibling.subsibling.somelogger");
    REQUIRE_THROWS_AS(registry.setBranchSeverity("subsibling", nealog::Severity::Trace),
                      nealog::UnregisteredKeyException);
}


TEST_CASE("create multiple logger on multiple threads", TAG_THREADING)
{
    constexpr int RUNS{10000};
    std::array<std::string, RUNS> t1_loggerNames, t2_loggerNames;

    LoggerTreeRegistry_mt registry;
    std::atomic<int> count = 0;

    std::thread threadOne([RUNS, &registry, &count, &t1_loggerNames]() {
        for (int i = 0; i < RUNS; i++)
        {
            t1_loggerNames[i] = std::to_string(count++);
            registry.getOrCreate(t1_loggerNames[i]);
        }
    });

    std::thread threadTwo([RUNS, &registry, &count, &t2_loggerNames]() {
        for (int i = 0; i < RUNS; i++)
        {
            t2_loggerNames[i] = std::to_string(count++);
            registry.getOrCreate(t2_loggerNames[i]);
        }
    });
    threadOne.join();
    threadTwo.join();
    requireResultEqualsExpected(count.load(), 2 * RUNS);
}
