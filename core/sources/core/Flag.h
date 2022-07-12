#pragma once

#include <cstring>
#include <vector>

// todo: create a static counter (see ref : https://blog.tartanllama.xyz/integral_variable/, https://b.atch.se/)

namespace Core
{
    class FlagBase
    {
    public:

        FlagBase() = delete;
        FlagBase(const FlagBase&) = default;
        FlagBase(FlagBase&&) noexcept = default;
        ~FlagBase() = default;

        FlagBase& operator=(const FlagBase&) = default;
        FlagBase& operator=(FlagBase&&) noexcept = default;

        FlagBase operator|(FlagBase other) const { return other.value |= value; }
        FlagBase& operator|=(const FlagBase other) { value |= other.value; return *this; }

        bool include(FlagBase flags) const { return value & flags.value; }

    protected:

        constexpr FlagBase(int value) : value(value) {}

    private:
        int value;

    };

    template<typename T>
    class Flag : public FlagBase
    {
    public:
        Flag() = delete;
        Flag(const Flag&) = default;
        Flag(Flag&&) noexcept = default;
        ~Flag() = default;

        Flag& operator=(const Flag&) = default;
        Flag& operator=(Flag&&) noexcept = default;

        T operator|(T other) const { return Flag((FlagBase)*this | other); }
        T& operator|=(T other) { *this = *this | other; return *this; }

        operator T() const { return *(T*)this; }

    protected:
        static Flag<T> NewFlag()
        {
            return Flag<T>(1 << nextPosition++);
        }

    private:
        class DefaultFlag
        {
        public:
            constexpr operator T() const
            {
                return Flag<T>(0);
            }
        };

        explicit Flag(FlagBase other) :
            FlagBase(other)
        {}

        constexpr Flag(int value) :
            FlagBase(value)
        {}

        inline static int nextPosition = 0;

    public:
        static inline const DefaultFlag NONE;

    };
}
