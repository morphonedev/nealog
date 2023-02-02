#pragma once

#include <string_view>

namespace nealog::utility
{

    auto beginsWith(const std::string_view haystack, std::string_view needle) -> bool
    {
        bool result{false};
        if (haystack.rfind(needle, 0) == 0) // pos=0 limits the search to the prefix
        {
            result = true;
        }
        return result;
    }
} // namespace nealog::utility
