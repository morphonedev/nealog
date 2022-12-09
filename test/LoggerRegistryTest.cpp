#include "nealog/Logger.h"
#include "nealog/Sink.h"
#include "TestApi.h"
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using namespace std::chrono_literals;
using namespace nealog;

constexpr const char* TAG             = "[LoggerRegistry]";
constexpr const char* TAG_INTEGRATION = "[LoggerRegistry][Integration]";
constexpr const char* TAG_THREADING   = "[LoggerRegistry][Multithreading]";


TEST_CASE("should create new logger", TAG)
{
    LoggerRegistry_st registry;
    auto logger = registry.getOrCreate("com");
    requirePointerNotNull(logger.get());
}



TEST_CASE("should get existing logger", TAG)
{
    LoggerRegistry_st registry;
    auto logger       = registry.getOrCreate("com");
    auto secondLogger = registry.getOrCreate("com");
    requireResultEqualsExpected(secondLogger, logger);
}



TEST_CASE("create multiple logger on multiple threads", TAG_THREADING)
{
    LoggerRegistry_mt registry;
    int count = 0;

    std::thread threadOne([&]() {
        for (int i = 0; i < 10000; i++)
        {
            registry.getOrCreate(std::to_string(count++));
        }
    });

    std::thread threadTwo([&]() {
        for (int i = 0; i < 10000; i++)
        {
            registry.getOrCreate(std::to_string(count++));
        }
    });
    threadOne.join();
    threadTwo.join();
    requireResultEqualsExpected(count, 20000);
}



TEST_CASE("should log to parent if logger has no sinks", TAG_INTEGRATION)
{
    // arrange
    const char* MESSAGE = "debug";
    LoggerRegistry_st registry;
    auto rootLogger = registry.getOrCreate("");
    std::ostringstream oss;
    rootLogger->addSink(SinkFactory::createStreamSink(oss));
    auto logger = registry.getOrCreate("com");

    // act
    logger->debug(MESSAGE);

    // assert
    requireResultEqualsExpected(oss.str(), MESSAGE);
}

auto createLoggerTree() -> void
{
    TestFacade facade;
    /* spans a logtree 
     *
     * [root]
     *   ↓
     *  appbar
     *   ↓
     *  menu
     *   ↓
     *  about
     */
    auto aboutLogger  = facade.getLogger("appbar.menu.about");
}

