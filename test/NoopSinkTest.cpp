#include "nealog/Logger.h"
#include "nealog/Sink.h"
#include <catch2/catch_test_macros.hpp>


using namespace nealog;


constexpr const char* TAG = "[Sink][NoopSink]";



TEST_CASE("NoopSink should have correct SinkType", TAG)
{
    NoopSink noop;
    REQUIRE(noop.getType() == SinkType::Noop);
}



// Here we only test that the NoopSink throws no exception.
TEST_CASE("Should not throw Exception and run without interference", TAG)
{
    NoopSink noop;
    REQUIRE_NOTHROW(noop.write(Severity::Debug, "message"));
    REQUIRE_NOTHROW(noop.flush());
}
