#pragma once

#include <string>

namespace nealog
{

    enum class Severity
    {
        Trace = 0,
        Debug = 1,
        Info  = 2,
        Warn  = 3,
        Error = 4,
        Fatal = 5
    };



    auto severityToString(Severity severity) -> const std::string;



    /*!
     * The severity is defaulted to Severity::Trace
     */
    class WithSeverity
    {
      protected:
        WithSeverity() = default;

      public:
        auto setSeverity(Severity) -> void;
        auto getSeverity() noexcept -> Severity;

      protected:
        Severity severity_ = Severity::Trace;
    };

} // namespace nealog

#ifdef NEALOG_HEADERONLY
#include "nealog_impl/SeverityImpl.h"
#endif // NEALOG_HEADERONLY
