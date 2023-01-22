#include "nealog/Logger.h"
#include "TestApi.h"
#include "nealog/Error.h"
#include "nealog/Formatter.h"
#include "nealog/Sink.h"
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <trompeloeil.hpp>
#include <vector>

using namespace nealog;

constexpr const char* TAG           = "[Logger]";
constexpr const char* TAG_THREADING = "[Logger][Multithreading]";

auto getTestLogger() -> std::unique_ptr<Logger>
{
    return std::make_unique<Logger>();
}



class MockLogger : public trompeloeil::mock_interface<LoggerBase>
{
    IMPLEMENT_MOCK1(addSink);
    IMPLEMENT_MOCK2(log);
};



class TestFixture
{
  public:
    TestFixture() : logger{}
    {
    }

  public:
    Logger logger;
};



TEST_CASE_METHOD(TestFixture, "should add sink to the logger", TAG)
{
    logger.addSink(SinkFactory::createStdOutSink());

    SECTION("adding sink removes the NoopSink if its the only sink existing")
    {
        // If the line above does add a sink and does not remove the noop
        // than there would be two items in the vector returned by getSinks()
        REQUIRE(logger.getSinks().size() == 1);
        REQUIRE(logger.getSinks().front()->getType() == SinkType::Stream);
    }
}



// The only sinks that should write the message are the ones that have a
// severity higher or equal to the logged message.
TEST_CASE_METHOD(TestFixture, "Only sinks with correct severity should write message", TAG)
{
    // create streams
    std::ostringstream debugSeverityStream{};
    std::ostringstream infoSeverityStream{};
    std::ostringstream errorSeverityStream{};

    // create the sinks
    auto debugSeveritySink = SinkFactory::createStreamSink(debugSeverityStream);
    debugSeveritySink->setSeverity(Severity::Debug);
    auto infoSeveritySink = SinkFactory::createStreamSink(infoSeverityStream);
    infoSeveritySink->setSeverity(Severity::Info);
    auto errorSeveritySink = SinkFactory::createStreamSink(errorSeverityStream);
    errorSeveritySink->setSeverity(Severity::Error);

    logger.addSink(debugSeveritySink);
    logger.addSink(infoSeveritySink);
    logger.addSink(errorSeveritySink);

    const char* MESSAGE = "warning message";

    logger.log(Severity::Warn, MESSAGE);

    CHECK(debugSeverityStream.str() == MESSAGE);
    CHECK(infoSeverityStream.str() == MESSAGE);
    CHECK(errorSeverityStream.str() == "");
}



constexpr const int MAX_LOG_MESSAGES = 1000;

void logOnFirstThread(LoggerBase* logger)
{
    for (int i = 0; i < MAX_LOG_MESSAGES; i++)
        logger->log(Severity::Info, "Message 1\n");
}

void logOnSecondThread(LoggerBase* logger)
{
    for (int i = 0; i < MAX_LOG_MESSAGES; i++)
        logger->log(Severity::Info, "Message 2\n");
}

TEST_CASE_METHOD(TestFixture, "Create different threads and write to both without missing a message", TAG_THREADING)
{
    std::stringstream stream{};
    logger.addSink(SinkFactory::createStreamSink(stream));

    std::thread firstThread(logOnFirstThread, &logger);
    std::thread secondThread(logOnFirstThread, &logger);

    firstThread.join();
    secondThread.join();

    std::string currentLine;
    const char SEPARATOR = '\n';
    std::vector<std::string> lines{};
    lines.reserve(20000);

    while (std::getline(stream, currentLine, SEPARATOR))
        lines.emplace_back(currentLine);

    CHECK(lines.size() == MAX_LOG_MESSAGES * 2);
}



TEST_CASE_METHOD(TestFixture, "Logging to convenience methods")
{
    const char* MESSAGE = "message";
    std::ostringstream stream{};
    logger.addSink(SinkFactory::createStreamSink(stream));

    SECTION("trace() creates trace message")
    {
        logger.trace(MESSAGE);
        REQUIRE(stream.str() == MESSAGE);
    }

    SECTION("debug() creates debug message")
    {
        logger.debug(MESSAGE);
        REQUIRE(stream.str() == MESSAGE);
    }

    SECTION("info() creates info message")
    {
        logger.info(MESSAGE);
        REQUIRE(stream.str() == MESSAGE);
    }

    SECTION("warn() creates warn message")
    {
        logger.warn(MESSAGE);
        REQUIRE(stream.str() == MESSAGE);
    }

    SECTION("error() creates error message")
    {
        logger.error(MESSAGE);
        REQUIRE(stream.str() == MESSAGE);
    }

    SECTION("fatal() creates fatal message")
    {
        logger.fatal(MESSAGE);
        REQUIRE(stream.str() == MESSAGE);
    }
}



auto getLoggerWithStreamSink(const std::ostream& stream) -> std::unique_ptr<Logger>
{
    auto logger = getTestLogger();
    logger->addSink(SinkFactory::createStreamSink(stream));
    return logger;
}


TEST_CASE("Logging a message with a lesser severity than set produces no output", TAG)
{
    std::ostringstream stream;
    auto logger = getLoggerWithStreamSink(stream);
    logger->setSeverity(Severity::Info);
    logger->log(Severity::Debug, "Message");
    REQUIRE(stream.str() == "");
}



TEST_CASE("Logging a message with an equal severity than set produces output", TAG)
{
    std::ostringstream stream;
    auto logger = getLoggerWithStreamSink(stream);
    logger->log(Severity::Info, "Message");
    REQUIRE(stream.str() == "Message");
}



TEST_CASE("Logging a message with a greater severity than set produces output", TAG)
{
    std::ostringstream stream;
    auto logger = getLoggerWithStreamSink(stream);
    logger->log(Severity::Error, "Message");
    REQUIRE(stream.str() == "Message");
}



TEST_CASE("all sinks should write the message", TAG)
{
    std::ostringstream firstStream;
    std::ostringstream secondStream;

    auto logger = getLoggerWithStreamSink(firstStream);
    logger->addSink(SinkFactory::createStreamSink(secondStream));
    logger->log(Severity::Error, "Message");

    REQUIRE(firstStream.str() == "Message");
    REQUIRE(secondStream.str() == "Message");
}



TEST_CASE("format output with formatter", TAG)
{
    std::ostringstream stream;
    auto logger = getLoggerWithStreamSink(stream);
    logger->log(Severity::Error, nlFormat("Message {}", "but with arg"));
    REQUIRE(stream.str() == "Message but with arg");
}



TEST_CASE("set formatter in logger and log to stream with pattern", TAG)
{
    std::ostringstream stream;
    auto logger = getLoggerWithStreamSink(stream);

    PatternFormatter formatter{"super %(message)"};
    logger->setFormatter(formatter);

    auto retrievedFormatter = logger->getFormatter();

    requireResultEqualsExpected(retrievedFormatter.getPattern(), formatter.getPattern());

    logger->log(Severity::Error, "Lorem");
    requireResultEqualsExpected(stream.str(), "super Lorem");
}
