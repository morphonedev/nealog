#pragma once

#include "nealog/Logger.h"
#include "catch2/catch_test_macros.hpp"
#include <memory>


class TestFacade
{
    nealog::LoggerRegistry_st registry;

  public:
    auto getLogger(const std::string_view& name) -> std::shared_ptr<nealog::LoggerBase>
    {
        auto rootLogger = registry.getOrCreate(name.data());
        return rootLogger;
    }
};



template <typename Result, typename Expected>
inline auto requireResultEqualsExpected(Result result, Expected expected) -> void
{
    REQUIRE(result == expected);
}

template <typename T>
inline auto requirePointerNotNull(T* pointer) -> void
{
    REQUIRE(pointer != nullptr);
}
