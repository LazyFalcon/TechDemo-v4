#include "core.hpp"
#include "Timer.hpp"
#include "ui.hpp"
#include "PerfCounter.hpp"
#include "Colors.hpp"

u32 PerfCounter::allocatedTextureMemory;
u32 PerfCounter::allocatedMeshMemory;
u32 PerfCounter::allocatedBufforsMemory;
u32 PerfCounter::allocatedTextures;
u32 PerfCounter::triangleCount;
u32 PerfCounter::lightsOnScene;
u32 PerfCounter::drawCalls;
std::unordered_map<std::string, u32> PerfCounter::records;

void showPerfCounter(UI::IMGUI &ui){
}
