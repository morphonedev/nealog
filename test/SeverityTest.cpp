#include "nealog/Severity.h"
#include "nealog/Error.h"

#include <catch2/catch_test_macros.hpp>

using namespace nealog;

constexpr Severity UNSUPPORTED_SEVERITY = (Severity)6;
constexpr const char* TAG = "[Severity]";

TEST_CASE("Parse Severity to string", TAG)
{
    SECTION("Trace")
    {
        std::string result = severityToString(Severity::Trace);
        REQUIRE(result == "Trace");
    }
    SECTION("Debug")
    {
        std::string result = severityToString(Severity::Debug);
        REQUIRE(result == "Debug");
    }
    SECTION("Info")
    {
        std::string result = severityToString(Severity::Info);
        REQUIRE(result == "Info");
    }
    SECTION("Warn")
    {
        std::string result = severityToString(Severity::Warn);
        REQUIRE(result == "Warn");
    }
    SECTION("Error")
    {
        std::string result = severityToString(Severity::Error);
        REQUIRE(result == "Error");
    }
    SECTION("Fatal")
    {
        std::string result = severityToString(Severity::Fatal);
        REQUIRE(result == "Fatal");
    }


    SECTION("Not supported throws ParseException")
    {
        REQUIRE_THROWS_AS(severityToString(UNSUPPORTED_SEVERITY), ParseException);
    }
}
