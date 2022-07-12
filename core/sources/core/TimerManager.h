#pragma once
#include <core_export.h>
#include <functional>
#include <vector>

namespace Core
{
    struct Timer
    {
        Timer(std::function<void()> otherFunction, float otherInterval, int otherLoop);

        bool    Tick(float delta);

        int     loop;
        float   interval;
        float   timeRemaining;

        std::function<void()>   function;
    };

    class TimerManager
    {
    public:
        ~TimerManager() = default;

        TimerManager(const TimerManager&) = delete;
        TimerManager(TimerManager&&) = delete;
        TimerManager& operator=(const TimerManager&) = delete;
        TimerManager& operator=(TimerManager&&) = delete;

        CORE_EXPORT void    _internal_CreateTimer(const std::function<void()>& function, float interval, int loop = -1);

        CORE_EXPORT void    Tick(float delta);

        CORE_EXPORT static TimerManager&    GetTimerManager();

    private:
        TimerManager() = default;

        std::vector<Timer>  timers;
    };
}

#define ID_TM(x) x
#define GET_MACRO_TM(_1, _2, _3, _4, NAME, ...) NAME
/*
 * CreateTimer(Function, Object, IntervalSeconds, LoopCount(optional))
 * LoopCount <= 0 means infinite number of loops
 */
#define CreateTimer(...) ID_TM(GET_MACRO_TM(__VA_ARGS__, CreateTimer3, CreateTimer2)(__VA_ARGS__))

#define CreateTimer3(FUNCTION, OBJECT, INTERVAL, LOOP) _internal_CreateTimer(std::bind(FUNCTION, OBJECT), INTERVAL, LOOP)
#define CreateTimer2(FUNCTION, OBJECT, INTERVAL) _internal_CreateTimer(std::bind(FUNCTION, OBJECT), INTERVAL)