#pragma once
#include "ui-core.hpp"
#include "ui-panel.hpp"
#include "ui-styler.hpp"
#include <vector>

class Panel;
class RenderedUIItems;

// Main class of this miracle
class Imgui
{
private:
    std::vector<Panel> m_panelStack;
    i32 m_width, m_height;
    std::unique_ptr<RenderedUIItems> m_renderedUIItems;

public:
    Styler basicStyle;

    Imgui(i32 width, i32 height, const std::string& name = "default");
    ~Imgui();

    Panel& panel(){ // returns active panel
        return m_panelStack.back();
    }

    Panel& newFixedPanel();
    void finishPanel(Panel*);

    void restart();

    void getKey(const glm::vec4& box){}

    RenderedUIItems& getToRender(){
        return *m_renderedUIItems;
    }
};
