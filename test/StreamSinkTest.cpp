#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include "nealog/Sink.h"
#include "typehelper.h"

using namespace nealog;

constexpr const char* TAG           = "[Sink][StreamSink]";
constexpr const char* TAG_THREADING = "[Sink][StreamSink][Multithreading]";



TEST_CASE("Create a StreamSink", TAG)
{
    std::stringstream outputStream;
    auto createdSink = SinkFactory::createStreamSink(outputStream);
    CHECK(isInstanceOf<StreamSink>(createdSink.get()));
}



// here the copy constructor copies the sink but does not copy the
// stream inside. Both sinks should write to the same stream.
TEST_CASE("Make sure copy constructor copies correctly", TAG)
{
    const char* EXPECTED_SOURCE_MESSAGE = "expected in source stream";
    const char* EXPECTED_DESTIN_MESSAGE = "expected in source stream + destination";

    auto sourceStream = std::stringstream{};
    auto sourceSink   = StreamSink(sourceStream);
    sourceSink.write(Severity::Info, EXPECTED_SOURCE_MESSAGE);
    REQUIRE(sourceStream.str() == EXPECTED_SOURCE_MESSAGE);

    StreamSink copiedSink{sourceSink};
    copiedSink.write(Severity::Info, " + destination");

    REQUIRE(sourceStream.str() == EXPECTED_DESTIN_MESSAGE);
}



TEST_CASE("Make sure copy assignment operator copies correctly", TAG)
{
    const char* EXPECTED_SOURCE_MESSAGE = "expected in source stream";
    const char* EXPECTED_DESTIN_MESSAGE = "expected in source stream + destination";

    auto sourceStream = std::stringstream();
    auto sourceSink   = StreamSink(sourceStream);
    sourceSink.write(Severity::Info, EXPECTED_SOURCE_MESSAGE);
    REQUIRE(sourceStream.str() == EXPECTED_SOURCE_MESSAGE);

    StreamSink copiedSink{std::ostringstream{}};
    copiedSink = sourceSink;
    copiedSink.write(Severity::Info, " + destination");

    REQUIRE(sourceStream.str() == EXPECTED_DESTIN_MESSAGE);
}



TEST_CASE("Move Constructor should move stream from one sink to another", TAG)
{
    const char* EXPECTED_SOURCE_MESSAGE = "expected in source stream";
    const char* EXPECTED_DESTIN_MESSAGE = "expected in source stream + destination";

    auto sourceStream        = std::stringstream();
    auto sourceSink          = StreamSink(sourceStream);
    auto addressofSourceSink = &sourceSink;
    sourceSink.write(Severity::Info, EXPECTED_SOURCE_MESSAGE);
    REQUIRE(sourceStream.str() == EXPECTED_SOURCE_MESSAGE);

    StreamSink movedSink(std::move(sourceSink));
    REQUIRE(sourceSink.getUnderlyingStream() == nullptr);
}



TEST_CASE("Move assignment operator should move stream from one sink to another", TAG)
{
    const char* EXPECTED_SOURCE_MESSAGE = "expected in source stream";
    const char* EXPECTED_DESTIN_MESSAGE = "expected in source stream + destination";

    auto sourceStream        = std::stringstream();
    auto sourceSink          = StreamSink(sourceStream);
    auto addressofSourceSink = &sourceSink;
    sourceSink.write(Severity::Info, EXPECTED_SOURCE_MESSAGE);
    REQUIRE(sourceStream.str() == EXPECTED_SOURCE_MESSAGE);


    StreamSink movedSink{std::ostringstream{}};
    movedSink = std::move(sourceSink);
    REQUIRE(sourceSink.getUnderlyingStream() == nullptr);
}



TEST_CASE("StreamSink should return the correct type", TAG)
{
    std::stringstream outputStream;
    auto createdSink = SinkFactory::createStreamSink(outputStream);
    CHECK(createdSink->getType() == SinkType::Stream);
}



TEST_CASE("Create StreamSink with standard IO streams")
{
    SECTION("std::cout")
    {
        REQUIRE_NOTHROW(SinkFactory::createStreamSink(std::cout));
    }
    SECTION("std::cerr")
    {
        REQUIRE_NOTHROW(SinkFactory::createStreamSink(std::cerr));
    }
}



TEST_CASE("Write to StreamSink and check output", TAG)
{
    std::stringstream outputStream;
    auto createdSink = SinkFactory::createStreamSink(outputStream);

    const char* STREAM_INPUT = "This should be in the stream";

    SECTION("Should write to underlying stream")
    {
        createdSink->write(Severity::Info, STREAM_INPUT);
        CHECK(outputStream.str() == STREAM_INPUT);
    }
}



TEST_CASE("Create a stdout sink", TAG)
{
    auto createdSink = SinkFactory::createStdOutSink();
    CHECK(isInstanceOf<StdOutSink>(createdSink.get()));
}



// This test can not be tested automatically because we
// cannot check the out put of std::cout.
// If the test passes the message is written to the console.
TEST_CASE("Write to stdout stream", TAG)
{
    auto createdSink = SinkFactory::createStdOutSink();
    createdSink->write(Severity::Info, "LOG FROM test \"Write to stdout stream\": "
                                       "Message that should appear at the console\n");
}



constexpr const int MAX_LOG_MESSAGES = 1000;
void logOnFirstThread(Sink* sink)
{
    for (int i = 0; i < MAX_LOG_MESSAGES; i++)
        sink->write(Severity::Info, "Message 1\n");
}

void logOnSecondThread(Sink* sink)
{
    for (int i = 0; i < MAX_LOG_MESSAGES; i++)
        sink->write(Severity::Info, "Message 2\n");
}

TEST_CASE("Create different threads and write to both without missing a message", TAG_THREADING)
{

    std::stringstream stream{};
    StreamSink sink(stream);
    std::thread firstThread(logOnFirstThread, &sink);
    std::thread secondThread(logOnFirstThread, &sink);
    firstThread.join();
    secondThread.join();

    std::string currentLine;
    const char SEPARATOR = '\n';
    std::vector<std::string> lines{};
    lines.reserve(2000);

    while (std::getline(stream, currentLine, SEPARATOR))
        lines.emplace_back(currentLine);

    CHECK(lines.size() == MAX_LOG_MESSAGES * 2);
}



TEST_CASE("Default severity for StreamSink is Trace", TAG)
{
    auto sink = SinkFactory::createStreamSink(std::ostringstream{});
    CHECK(sink->getSeverity() == Severity::Trace);
}
