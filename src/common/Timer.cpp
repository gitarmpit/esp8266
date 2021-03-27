#include "Timer.h"

Timer::Timer()
{
    Reset();
}

void Timer::Reset()
{
    _t_start = std::chrono::steady_clock::now();
}

int Timer::ElapsedMs()
{
    auto t_current = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t_current - _t_start);
    return (int)dur.count();
}