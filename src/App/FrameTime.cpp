#include "core.hpp"
#include "FrameTime.hpp"

std::chrono::time_point<std::chrono::high_resolution_clock> FrameTime::timeOnStart;
std::chrono::time_point<std::chrono::high_resolution_clock> FrameTime::lastTimePoint;
u64 FrameTime::miliseconds = 0;
u64 FrameTime::nanoseconds = 0;
u64 FrameTime::delta = 0;
float FrameTime::deltaf = 0;
