#include "ui-rendered.hpp"
#include "ui.hpp"
#include "Logging.hpp"

Imgui::~Imgui() = default;

Imgui::Imgui(i32 width, i32 height, const std::string& name):
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
    // m_defaultPanel.width(m_width).height(m_height).layout().dummy();
    m_defaultPanel.reset();
}
