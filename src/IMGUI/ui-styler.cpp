#include "ui-styler.hpp"
#include "ui-panel.hpp"
#include "ui-items.hpp"
#include "ui-rendered.hpp"


void Styler::render(Panel& panel){
    m_renderedUiItems.put<RenderedUIItems::Background>(panel.m_background);
}
void Styler::render(Item& item){
    m_renderedUiItems.put<RenderedUIItems::Background>({item.m_size, 0, -0.1f, 0xf0f0f0ff});
}
void Styler::renderText(Item& item, const std::string& text){
    item.m_text->formatting = item.m_formatting.value_or(Text::Left);
    item.m_text->color = item.m_textColor.value_or(0x000000ff);
    item.m_text->font = item.m_font.value_or("ui_20_bold");
    item.m_text->depth = -0.15f;
    item.m_text->bounds = item.m_size + glm::vec4(5,5, -10, -10);
    item.m_text->renderTo(m_renderedUiItems.get<Text::Rendered>(), text);
}
