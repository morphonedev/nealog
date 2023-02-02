#pragma once

#include "nealog/Logger.h"
#include "catch2/catch_test_macros.hpp"
#include <memory>



template <typename Result, typename Expected>
inline auto requireResultEqualsExpected(Result result, Expected expected) -> void
{
    REQUIRE(result == expected);
}

template <typename Result, typename Expected>
inline auto requireResultNotEqualsExpected(Result result, Expected expected) -> void
{
    REQUIRE(result != expected);
}

template <typename T>
inline auto requirePointerNotNull(T* pointer) -> void
{
    REQUIRE(pointer != nullptr);
}
