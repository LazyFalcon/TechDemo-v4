#pragma once
#include <functional>
#include "Colors.hpp"
#include "PerfTimers.hpp"

namespace UI
{
class IMGUI;
}
/**
 *  cel tego czegos:
 *   > wywietlanie debugowych informacji z counterów
 *   > różne debug ustawienia (jak je potem wyciagać?)
 *   > debugowa mapka z quadTree
 *
 */

class DebugScreen
{
public:
    void init();
    void enable() {
        enabled = !enabled;
    }
    void options(UI::IMGUI& ui);
    void show(UI::IMGUI& ui, bool ctrlPressed);

    static void addPanel(void*, std::function<void(UI::IMGUI&)>);
    static void removePanel(void*);

private:
    bool enabled {true};
    bool enableOptions {false};
    u64 enable3DMap {0};
    u64 enable2DMap {0};

    bool show3dQuadtree {false};
    glm::vec4 mapSize {0, 0, 256, 256};

    static std::map<void*, std::function<void(UI::IMGUI&)>> panels;
};
