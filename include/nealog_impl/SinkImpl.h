#pragma once

#ifndef DROELF_HEADERONLY
#include "nealog/Sink.h"
#endif // !DROELF_HEADERONLY
 
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>


namespace nealog
{
    constexpr const char* SINKTYPE_NOT_SUPPORTED = "The given Sinktype is not supported";

    DLF_INLINE UnsupportedSinkTypeException::UnsupportedSinkTypeException() : std::runtime_error(SINKTYPE_NOT_SUPPORTED)
    {
    }



    DLF_INLINE auto SinkFactory::createStreamSink(const std::ostream& outputStream) -> std::shared_ptr<StreamSink>
    {
        auto sink = std::make_shared<StreamSink>(outputStream);
        return sink;
    }



    DLF_INLINE auto SinkFactory::createStdOutSink() -> std::shared_ptr<StdOutSink>
    {
        auto sink = std::make_shared<StdOutSink>();
        return sink;
    }



    /******************************
     * NoopSink
     ******************************/
    DLF_INLINE auto NoopSink::getType() -> SinkType
    {
        return SinkType::Noop;
    }



    DLF_INLINE auto NoopSink::write(Severity severity, std::string_view message) -> void
    {
        // NoopSink does nothing
    }



    DLF_INLINE auto NoopSink::flush() -> void
    {
        // NoopSink does nothing
    }



    /******************************
     * StreamSink
     ******************************/
    //{{{

    DLF_INLINE StreamSink::StreamSink(const std::ostream& outputStream)
    {
        stream_ = std::make_shared<std::ostream>(outputStream.rdbuf());
    }



    DLF_INLINE StreamSink::StreamSink(StreamSink& source) : stream_(source.stream_)
    {
        setSeverity(source.severity_);
    }



    DLF_INLINE StreamSink::StreamSink(StreamSink&& source) noexcept
    {
        stream_ = std::move(source.stream_);
        setSeverity(source.severity_);
    }



    DLF_INLINE auto StreamSink::operator=(StreamSink& source) noexcept -> StreamSink&
    {
        stream_ = source.stream_;
        setSeverity(source.severity_);
        return *this;
    }



    DLF_INLINE auto StreamSink::operator=(StreamSink&& source) noexcept -> StreamSink&
    {
        stream_ = std::move(source.stream_);
        setSeverity(source.severity_);
        return *this;
    }



    DLF_INLINE auto StreamSink::getType() -> SinkType
    {
        return SinkType::Stream;
    }



    DLF_INLINE auto StreamSink::write(Severity messageSeverity, std::string_view message) -> void
    {
        mutex_.lock();
        if (messageSeverity >= severity_)
        {
            stream_->write(message.data(), message.size());
        }
        mutex_.unlock();
    }



    DLF_INLINE auto StreamSink::flush() -> void
    {
        mutex_.lock();
        stream_->flush();
        mutex_.unlock();
    }



    DLF_INLINE auto StreamSink::getUnderlyingStream() const -> std::shared_ptr<std::ostream>
    {
        return stream_;
    }

    //}}}



    /******************************
     * StdOutSink
     ******************************/
    DLF_INLINE StdOutSink::StdOutSink() : StreamSink(std::cout)
    {
    }

} // namespace nealog
