#pragma once

#include "nealog/Severity.h"
#include <memory>
#include <mutex>
#include <ostream>
#include <stdexcept>
#include <streambuf>
#include <string_view>
#include <type_traits>

namespace nealog
{

    enum class SinkType
    {
        Noop,
        Stream,
    };



    class UnsupportedSinkTypeException : public std::runtime_error
    {
      public:
        UnsupportedSinkTypeException();
    };



    /*!
     * Abstract base class of a logger output
     */
    class Sink : public WithSeverity
    {
      public:
        using SPtr = std::shared_ptr<Sink>;
        using UPtr = std::unique_ptr<Sink>;

      public:
        virtual ~Sink()                                        = default;
        virtual auto getType() -> SinkType                     = 0;
        virtual auto write(Severity, std::string_view) -> void = 0;
        virtual auto flush() -> void                           = 0;

      protected:
        std::mutex mutex_;
    };


    class NoopSink : public Sink
    {
      public:
        auto getType() -> SinkType override;
        auto write(Severity, std::string_view) -> void override;
        auto flush() -> void override;
    };



    class StreamSink : public Sink
    {
      public:
        StreamSink(const std::ostream& outputStream);
        StreamSink(StreamSink&);
        StreamSink(StreamSink&&) noexcept;

      public:
        auto operator=(StreamSink&) noexcept -> StreamSink&;
        auto operator=(StreamSink&&) noexcept -> StreamSink&;

      public:
        auto getType() -> SinkType override;
        auto write(Severity, std::string_view) -> void override;
        auto flush() -> void override;
        auto getUnderlyingStream() const -> std::shared_ptr<std::ostream>;

      private:
        std::shared_ptr<std::ostream> stream_ = nullptr;
    };



    class StdOutSink : public StreamSink
    {
      public:
        StdOutSink();
    };



    class SinkFactory
    {
      private:
        SinkFactory() = default;

      public:
        static auto createStreamSink(const std::ostream&) -> std::shared_ptr<StreamSink>;
        static auto createStdOutSink() -> std::shared_ptr<StdOutSink>;
    };

} // namespace nealog

#ifdef NEALOG_HEADERONLY
#include "nealog_impl/SinkImpl.h"
#endif // NEALOG_HEADERONLY
