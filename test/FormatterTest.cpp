#include "nealog/Formatter.h"
#include "TestApi.h"
#include "catch2/matchers/catch_matchers.hpp"

#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <iterator>

using namespace nealog;

constexpr const char* TAG = "[Formatter]";



class FormatterTestFixture
{
  public:
    FormatterTestFixture()
    {
    }

    Formatter formatter;
};

//{{{

TEST_CASE_METHOD(FormatterTestFixture, "should concat strings", TAG)
{
    const std::string EXPECTED = "Hello world";
    std::string result         = formatter.format("Hello {}", "world");
    requireResultEqualsExpected(result, EXPECTED);
}



TEST_CASE_METHOD(FormatterTestFixture, "should concat string and int", TAG)
{
    const std::string EXPECTED = "one is 1";
    std::string result         = formatter.format("one is {}", 1);
    requireResultEqualsExpected(result, EXPECTED);
}



TEST_CASE_METHOD(FormatterTestFixture, "should concat string and float", TAG)
{
    const std::string EXPECTED = "one is 1.0";
    std::string result         = formatter.format("one is {:.1f}", 1.0f);
    requireResultEqualsExpected(result, EXPECTED);
}



// learning test
TEST_CASE("replace all occurrences of a string inside another string", TAG)
{
    std::string test = "%(message)foobar mi%(message)nim sint cillum sint consectetur cupidatat.";
    std::string substitutor{"%(message)"};

    auto position = test.find(substitutor);

    while (position != std::string::npos)
    {
        test.replace(position, substitutor.size(), "themessage");
        position = test.find(substitutor);
    }

    CHECK_THAT(test,
               Catch::Matchers::Equals("themessagefoobar mithemessagenim sint cillum sint consectetur cupidatat."));
}

//}}}



/******************************
 * PatternFormatter
 ******************************/

//{{{

TEST_CASE("PatternFormatter with empty pattern returns message", TAG)
{
    std::string expected{"hello mum"};
    PatternFormatter formatter("");

    std::string result = formatter.format(expected);

    requireResultEqualsExpected(result, expected);
}



TEST_CASE("PatternFormatter with set pattern returns formatted message", TAG)
{
    std::string substitutor{MESSAGE_SUBSTITUTOR};
    std::string expected{"oh hello, mum"};

    PatternFormatter formatter("oh " + substitutor);

    std::string result = formatter.format("hello{} mum", ",");

    requireResultEqualsExpected(result, expected);
}

//}}}
