#pragma once
#include <chrono>

class Timer
{
public:
    Timer();
    virtual void Reset();
    virtual int ElapsedMs();

private:
    std::chrono::steady_clock::time_point _t_start;

};