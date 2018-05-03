#include "ui-rendered.hpp"
#include "ui.hpp"
#include "Logging.hpp"

Imgui::~Imgui() = default;

Imgui::Imgui(i32 width, i32 height, const std::string& name):
    m_width(width),
    m_height(height),
    m_renderedUIItems(std::make_unique<RenderedUIItems>()),
    basicStyle(*m_renderedUIItems)
{
    m_panelStack.reserve(30);
    m_panelStack.emplace_back(*this, &basicStyle, nullptr);
    auto& panel = m_panelStack.back();
    panel.width(m_width).height(m_height).layout().dummy();
}

void Imgui::restart(){
    if(m_panelStack.size() != 1){
        throw InvaliUiOperation("Not all panels were finished!");
    }
    m_renderedUIItems->reset();
    // cleanup all states

    input.main.on = false;
    if(input.main.off) input.main.position.reset();
    input.main.off = false;
    input.alternate.on = false;
    if(input.alternate.off) input.alternate.position.reset();
    input.alternate.off = false;

    input.cursorDepthInLastFrame = input.cursorDepthInThisFrame;
    input.cursorDepthInThisFrame = 0;
    input.mouseTranslation = glm::vec2(0);

    // recreate window panel, easier than reset function
    m_panelStack.clear();
    m_panelStack.emplace_back(*this, &basicStyle);
    m_panelStack.back().width(m_width).height(m_height).layout().dummy();
}

Panel& Imgui::newFixedPanel(){
    if(m_panelStack.size() != 1) throw InvaliUiOperation("There is somewhere unfinished Panel!");
    auto& last = m_panelStack.back();
    return last.newFixedPanel();
}
Panel& Imgui::instantiateNewFixedPanel(){
    auto& last = m_panelStack.back();
    m_panelStack.emplace_back(*this, &last.getStyler(), &last);
    return m_panelStack.back();
}

void Imgui::finishPanel(Panel* p){
    if(p != &m_panelStack.back()) log("wrong panel finished!");
    m_panelStack.pop_back();
}
