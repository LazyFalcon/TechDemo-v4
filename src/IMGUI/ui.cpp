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
