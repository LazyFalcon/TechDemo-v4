#include "ui-styler.hpp"
#include "ui-panel.hpp"
#include "ui-items.hpp"
#include "ui-rendered.hpp"
#include "Logging.hpp"


void Styler::render(Panel& panel){
    clog("Panel:", panel.m_background.box, panel.m_background.depth);
    m_renderedUiItems.put<RenderedUIItems::Background>(panel.m_background);
}
void Styler::render(Item& item){
    clog("Item:",item.m_size, item.m_depth);
    m_renderedUiItems.put<RenderedUIItems::ColoredBox>({item.m_size, item.m_depth, 0xf0f0f0f0});
}
void Styler::renderText(Item& item, const std::string& text){
    item.m_text->formatting = item.m_formatting.value_or(Text::Left);
    item.m_text->color = item.m_textColor.value_or(0x000000ff);
    item.m_text->font = item.m_font.value_or("ui_20_bold");
    item.m_text->depth = item.m_depth;
    item.m_text->bounds = item.m_size + glm::vec4(5,5, -10, -10);
    item.m_text->renderTo(m_renderedUiItems.get<Text::Rendered>(), text);
}