#include "RenderedUI.hpp"
#include "Imgui.hpp"
#include "Logging.hpp"

Imgui::~Imgui() = default;

Imgui::Imgui(i32 width, i32 height, const std::string& name):
    m_width(width),
    m_height(height),
    m_renderedUi(std::make_unique<RenderedUI>())
{
    m_panelStack.reserve(30);
    m_panelStack.emplace_back(*this);
    auto& panel = m_panelStack.back();
    panel.width(m_width).height(m_height);
}

void Imgui::restart(){
    if(m_panelStack.size() != 1){
        throw InvaliUiOperation("Not all panels were finished!");
    }
    m_renderedUi->reset();
    // cleanup all states
}

Panel& Imgui::newFixedPanel(){
    auto& last = m_panelStack.back();
    m_panelStack.emplace_back(*this, &last);
    return m_panelStack.back();
}

void Imgui::finishPanel(Panel* p){
    if(p != &m_panelStack.back()) log("wrong panel finished!");
    m_panelStack.pop_back();
}
