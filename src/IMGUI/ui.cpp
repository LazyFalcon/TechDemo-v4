#include "ui-rendered.hpp"
#include "ui.hpp"
#include "Logging.hpp"

Imgui::~Imgui() = default;

Imgui::Imgui(int width, int height, const std::string& name):
    m_width(width),
    m_height(height),
    m_renderedUIItems(std::make_unique<RenderedUIItems>()),
    basicStyle(*m_renderedUIItems),
    m_defaultPanel(this, &basicStyle, nullptr)
{
    m_defaultPanel.width(m_width).height(m_height).layout().dummy();
}

void Imgui::restart(){
    m_renderedUIItems->reset();
    // cleanup all states

    input.lmb.on = false;
    if(input.lmb.off) input.lmb.position.reset();
    input.lmb.off = false;
    input.rmb.on = false;
    if(input.rmb.off) input.rmb.position.reset();
    input.rmb.off = false;

    input.cursorDepthInLastFrame = input.cursorDepthInThisFrame;
    input.cursorDepthInThisFrame = 0;
    input.mouseTranslation = glm::vec2(0);

    // recreate window panel, easier than reset function
    // m_defaultPanel.width(m_width).height(m_height).layout().dummy();
    m_defaultPanel.reset();
}
/*
    Action to be performed on item should have been pressed on 'on' and on 'out', otherwise it doesn't count
*/
PointerActions Imgui::getPointerAction(const glm::vec4& poly, float depth){
    auto out = PointerActions::None;
    if(input.hover(poly, depth)) out = PointerActions::Hover;

    if(out == PointerActions::Hover and input.lmb.pressed(poly, depth)){
        if(input.lmb.on) return PointerActions::LmbOn;
        if(input.lmb.off) return PointerActions::LmbOff;
        out = PointerActions::LmbHold;
    }

    if(out == PointerActions::Hover and input.rmb.pressed(poly, depth)){
        if(input.rmb.on) return PointerActions::RmbOn;
        if(input.rmb.off) return PointerActions::RmbOff;
        out = PointerActions::RmbHold;
    }

    if(out == PointerActions::None and input.rmb.on or input.rmb.off or input.lmb.on or input.lmb.off)
        return PointerActions::ActionOutside;

    return out;
}
