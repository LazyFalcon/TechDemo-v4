#pragma once
#include <chrono>
struct FrameTime
{
    static std::chrono::time_point<std::chrono::high_resolution_clock> timeOnStart;
    static std::chrono::time_point<std::chrono::high_resolution_clock> lastTimePoint;

    static uint64_t miliseconds;
    static uint64_t nanoseconds;
    static uint64_t delta;
    static float deltaf;
};
