#pragma once
#include "ImguiCore.hpp"
#include "Panel.hpp"
#include "Style.hpp"
#include <vector>

class Panel;

// Main class of this miracle
class Imgui
{
public:
    Imgui(u32 width, u32 height, const std::string& name = "default"):
        m_width(width),
        m_height(height)
    {
        m_panelStack.reserve(30);
        m_panelStack.emplace_back(*this);
    }

    Panel& panel(){ // returns active panel
        return m_panelStack.back();
    }

    Panel& newFixedPanel();

    void reset();

    std::vector<ImguiRenderElement>& getToRender(){
        return m_style.renderedElements;
    }
private:
    std::vector<Panel> m_panelStack;
    i32 m_width, m_height;
    Style m_style;
};
