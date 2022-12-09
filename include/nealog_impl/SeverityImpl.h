#pragma once

#ifndef DROELF_HEADERONLY
#include "nealog/Severity.h"
#endif // !DROELF_HEADERONLY

#include "nealog/Error.h"


namespace nealog
{

    constexpr const char* SEVERITY_PARSE_ERROR = "The given severity could not be parsed";



    DLF_INLINE auto severityToString(Severity severity) -> const std::string
    {
        switch (severity)
        {
        case Severity::Trace:
            return "Trace";
        case Severity::Debug:
            return "Debug";
        case Severity::Info:
            return "Info";
        case Severity::Warn:
            return "Warn";
        case Severity::Error:
            return "Error";
        case Severity::Fatal:
            return "Fatal";
        default:
            throw ParseException(SEVERITY_PARSE_ERROR);
        }
    }



    /******************************
     * WithSeverity
     ******************************/

    DLF_INLINE auto WithSeverity::setSeverity(Severity severity) -> void
    {
        severity_ = severity;
    }



    DLF_INLINE auto WithSeverity::getSeverity() noexcept -> Severity
    {
        return severity_;
    }

} // namespace nealog
