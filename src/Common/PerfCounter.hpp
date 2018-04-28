#pragma once
#include "common.hpp"
#include "Timer.hpp"
#include "Logging.hpp"
namespace UI { class IMGUI; }

class PerfCounter
{
public:
    static std::unordered_map<std::string, u32> records;
    // memory
    static u32 allocatedTextureMemory;
    static u32 allocatedMeshMemory;
    static u32 allocatedBufforsMemory;
    // resources
    static u32 allocatedTextures;
    static u32 triangleCount;
    static u32 lightsOnScene;
    static u32 drawCalls;
};

void showPerfCounter(UI::IMGUI &ui);
