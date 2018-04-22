#include "RenderedUI.hpp"
#include "Imgui.hpp"

Imgui::~Imgui() = default;

Imgui::Imgui(u32 width, u32 height, const std::string& name):
    m_width(width),
    m_height(height),
    m_renderedUi(std::make_unique<RenderedUI>())
{
    m_panelStack.reserve(30);
    m_panelStack.emplace_back(*this);
}

void Imgui::reset(){
    if(m_panelStack.size() != 1){
        throw InvaliUiOperation("Not all panels were finished!");
    }
    m_renderedUi.reset();
    // cleanup all states
}

Panel& Imgui::newFixedPanel(){
    m_panelStack.emplace_back(*this, &m_panelStack.back());
    return m_panelStack.back();
}
