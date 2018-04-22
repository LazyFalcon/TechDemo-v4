#pragma once
#include "ImguiCore.hpp"
#include "Panel.hpp"
#include "Style.hpp"
#include <vector>

class Panel;
class RenderedUI;

// Main class of this miracle
class Imgui
{
public:
    Imgui(u32 width, u32 height, const std::string& name = "default");
    ~Imgui();

    Panel& panel(){ // returns active panel
        return m_panelStack.back();
    }

    Panel& newFixedPanel();

    void reset();

    RenderedUI& getToRender(){
        return *m_renderedUi;
    }
private:
    std::vector<Panel> m_panelStack;
    i32 m_width, m_height;
    Style m_style;
    std::unique_ptr<RenderedUI> m_renderedUi;
};
