#pragma once

#include <stdexcept>


namespace nealog
{

    class ParseException : public std::runtime_error
    {
      public:
        using std::runtime_error::runtime_error;
    };

} // namespace nealog
