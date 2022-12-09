#pragma once

#ifndef NEALOG_HEADERONLY
#include "nealog/Sink.h"
#endif // !NEALOG_HEADERONLY
 
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>


namespace nealog
{
    constexpr const char* SINKTYPE_NOT_SUPPORTED = "The given Sinktype is not supported";

    NL_INLINE UnsupportedSinkTypeException::UnsupportedSinkTypeException() : std::runtime_error(SINKTYPE_NOT_SUPPORTED)
    {
    }



    NL_INLINE auto SinkFactory::createStreamSink(const std::ostream& outputStream) -> std::shared_ptr<StreamSink>
    {
        auto sink = std::make_shared<StreamSink>(outputStream);
        return sink;
    }



    NL_INLINE auto SinkFactory::createStdOutSink() -> std::shared_ptr<StdOutSink>
    {
        auto sink = std::make_shared<StdOutSink>();
        return sink;
    }



    /******************************
     * NoopSink
     ******************************/
    NL_INLINE auto NoopSink::getType() -> SinkType
    {
        return SinkType::Noop;
    }



    NL_INLINE auto NoopSink::write(Severity severity, std::string_view message) -> void
    {
        // NoopSink does nothing
    }



    NL_INLINE auto NoopSink::flush() -> void
    {
        // NoopSink does nothing
    }



    /******************************
     * StreamSink
     ******************************/
    //{{{

    NL_INLINE StreamSink::StreamSink(const std::ostream& outputStream)
    {
        stream_ = std::make_shared<std::ostream>(outputStream.rdbuf());
    }



    NL_INLINE StreamSink::StreamSink(StreamSink& source) : stream_(source.stream_)
    {
        setSeverity(source.severity_);
    }



    NL_INLINE StreamSink::StreamSink(StreamSink&& source) noexcept
    {
        stream_ = std::move(source.stream_);
        setSeverity(source.severity_);
    }



    NL_INLINE auto StreamSink::operator=(StreamSink& source) noexcept -> StreamSink&
    {
        stream_ = source.stream_;
        setSeverity(source.severity_);
        return *this;
    }



    NL_INLINE auto StreamSink::operator=(StreamSink&& source) noexcept -> StreamSink&
    {
        stream_ = std::move(source.stream_);
        setSeverity(source.severity_);
        return *this;
    }



    NL_INLINE auto StreamSink::getType() -> SinkType
    {
        return SinkType::Stream;
    }



    NL_INLINE auto StreamSink::write(Severity messageSeverity, std::string_view message) -> void
    {
        mutex_.lock();
        if (messageSeverity >= severity_)
        {
            stream_->write(message.data(), message.size());
        }
        mutex_.unlock();
    }



    NL_INLINE auto StreamSink::flush() -> void
    {
        mutex_.lock();
        stream_->flush();
        mutex_.unlock();
    }



    NL_INLINE auto StreamSink::getUnderlyingStream() const -> std::shared_ptr<std::ostream>
    {
        return stream_;
    }

    //}}}



    /******************************
     * StdOutSink
     ******************************/
    NL_INLINE StdOutSink::StdOutSink() : StreamSink(std::cout)
    {
    }

} // namespace nealog
