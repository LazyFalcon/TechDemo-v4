#pragma once
#include "common.hpp"
#include "Timer.hpp"
#include <queue>

typedef u64 timeType;

struct TimeRecord // in 1e-6s
{
    timeType last {0};
    timeType max {0};
    timeType total {0};
    timeType captured {0};
    u64 count {0};
    void update(timeType dt);
};

class CpuTimerScoped
{
public:
    CpuTimerScoped(const std::string &name);
    CpuTimerScoped(const std::string &name, i32 line);
    ~CpuTimerScoped();
    void between(i32 line);

    static std::map<std::string, TimeRecord> cpuRecords;
    static void writeToFile();
    static bool printRecords;
    static bool saveRecords;
private:
    const std::string name;
    Timer<timeType, 1'000'000, 1> timer;
};
#define CPU_SCOPE_TIMER(name) CpuTimerScoped _scopeTimer(name);
#define CPU_SCOPE_TIMER_UNNAMED() CpuTimerScoped _scopeTimer(__PRETTY_FUNCTION__);

class GpuTimerScoped
{
public:
    GpuTimerScoped(const std::string &name);
    ~GpuTimerScoped();
    static void init();
    static void print();
    static void writeToFile();
    static std::map<std::string, std::pair<u32, TimeRecord>> gpuRecords;
private:
    static std::queue<u32> freeTimerIds;
};
#define GPU_SCOPE_TIMER() GpuTimerScoped _gpuScopeTimer(__FUNCTION__);
