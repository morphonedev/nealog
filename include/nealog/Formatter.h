#pragma once

#include "fmt/compile.h"
#include <algorithm>
#include <fmt/core.h>
#include <utility>


#define nlFormat(msg, ...) nealog::Formatter().format(msg, __VA_ARGS__)

namespace nealog
{


    /*!
     * The formatter is a wrapper around the Fmt library
     */
    class Formatter
    {
      public:
        Formatter() = default;

        template <typename... TArg>
        auto format(const std::string_view& msg, TArg&&... args) -> std::string
        {
            return fmt::format(msg, std::forward<TArg>(args)...);
        }
    };



    constexpr const char* MESSAGE_SUBSTITUTOR = "%(message)";

    class PatternFormatter : public Formatter
    {
        using Formatter::format;

      public:
        PatternFormatter(const std::string_view& pattern);


        /*!
         *  If pattern_ is unset the message is returned as the normal Formatter formats it.
         */
        template <typename... TArg>
        auto format(const std::string_view& msg, TArg&&... args) -> std::string
        {
            std::string messageToFormat{msg};

            if (!pattern_.empty())
            {
                messageToFormat = wrapMessageWithPattern(msg);
            }

            return Formatter::format(messageToFormat, std::forward<TArg>(args)...);
        }

        auto getPattern() const -> const std::string&;

      private:
        auto wrapMessageWithPattern(const std::string_view& msg) -> std::string;

      private:
        std::string pattern_{};
    };


} // namespace nealog
  //
#ifdef NEALOG_HEADERONLY
#include "nealog_impl/FormatterImpl.h"
#endif // NEALOG_HEADERONLY
