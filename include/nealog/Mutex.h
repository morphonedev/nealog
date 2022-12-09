#pragma once

#include <mutex>

namespace nealog
{

    class MutexWrapper
    {
      public:
        virtual auto lock() -> void   = 0;
        virtual auto unlock() -> void = 0;
    };


    class FakeMutex : public MutexWrapper
    {
      public:
        auto lock() -> void
        {
        }

        auto unlock() -> void
        {
        }
    };



    class RealMutex : public MutexWrapper
    {
      public:
        auto lock() -> void
        {
            mutex_.lock();
        }

        auto unlock() -> void
        {
            mutex_.unlock();
        }

      private:
        std::recursive_mutex mutex_;
    };

} // namespace nealog
