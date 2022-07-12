#include "TimerManager.h"

#include <utility>

namespace Core
{
    Timer::Timer(std::function<void()> otherFunction, const float otherInterval, const int otherLoop) :
        loop(otherLoop), interval(otherInterval), timeRemaining(otherInterval), function(std::move(otherFunction))
    {}

    bool Timer::Tick(const float delta)
    {
        timeRemaining -= delta;

        while (timeRemaining <= 0)
        {
            function();
            timeRemaining += interval;
            loop--;

            if (loop == 0)
            {
                return true;
            }
        }

        return false;
    }

    void TimerManager::_internal_CreateTimer(const std::function<void()>& function, const float interval, const int loop)
    {
        timers.emplace_back(function, interval, loop);
    }

    void TimerManager::Tick(const float delta)
    {
        for (auto it = timers.begin(); it != timers.end();)
        {
            if (it->Tick(delta))
            {
                it = timers.erase(it);
            }
            else
                ++it;
        }
    }

    TimerManager& TimerManager::GetTimerManager()
    {
        static TimerManager timerManager;
        return timerManager;
    }
}


