#pragma once

#ifndef NEALOG_HEADERONLY
#include "nealog/Formatter.h"
#endif // !NEALOG_HEADERONLY

#include <string>

namespace nealog
{
    NL_INLINE PatternFormatter::PatternFormatter(const std::string_view& pattern) : pattern_{pattern}
    {
    }



    NL_INLINE auto PatternFormatter::wrapMessageWithPattern(const std::string_view& msg) -> std::string
    {
        std::string copiedPattern{pattern_};
        size_t position = copiedPattern.find(MESSAGE_SUBSTITUTOR);

        while (position != std::string::npos)
        {
            copiedPattern.replace(position, strlen(MESSAGE_SUBSTITUTOR), msg);
            position = copiedPattern.find(MESSAGE_SUBSTITUTOR);
        }

        return copiedPattern;
    }

    NL_INLINE auto PatternFormatter::getPattern() const -> const std::string&
    {
        return pattern_;
    }
} // namespace nealog
