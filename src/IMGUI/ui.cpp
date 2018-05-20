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
    input.reset();
    panelInput.reset();

    m_defaultPanel.reset();
}

bool ItemActions::hover(const glm::vec4& poly, float depth){
    bool hasHover = (mousePos.x>=poly.x and mousePos.x <=poly.x+poly.z) and (mousePos.y >=poly.y and mousePos.y <=poly.y+poly.w);
    if(hasHover and depth >= cursorDepthInLastFrame){
        cursorDepthInThisFrame = depth;
        return true;
    }
    return false;
}
void ItemActions::reset(){
    lmb.on = false;
    if(lmb.off) lmb.position.reset();
    lmb.off = false;
    rmb.on = false;
    if(rmb.off) rmb.position.reset();
    rmb.off = false;

    cursorDepthInLastFrame = cursorDepthInThisFrame;
    cursorDepthInThisFrame = 0;
    mouseTranslation = glm::vec2(0);
}
/*
    Action to be performed on item should have been pressed on 'on' and on 'out', otherwise it doesn't count
*/
PointerActions ItemActions::getPointerAction(const glm::vec4& poly, float depth){
    auto out = PointerActions::None;
    if(hover(poly, depth)) out = PointerActions::Hover;

    if(out == PointerActions::Hover and lmb.pressed(poly, depth)){
        if(lmb.on) return PointerActions::LmbOn;
        if(lmb.off) return PointerActions::LmbOff;
        out = PointerActions::LmbHold;
    }

    if(out == PointerActions::Hover and rmb.pressed(poly, depth)){
        if(rmb.on) return PointerActions::RmbOn;
        if(rmb.off) return PointerActions::RmbOff;
        out = PointerActions::RmbHold;
    }

    if(out == PointerActions::None and rmb.on or rmb.off or lmb.on or lmb.off)
        return PointerActions::ActionOutside;

    return out;
}
