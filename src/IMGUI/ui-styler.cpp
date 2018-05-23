#include "ui-styler.hpp"
#include "ui-panel.hpp"
#include "ui-items.hpp"
#include "ui-rendered.hpp"


void Styler::render(Panel& panel){
    if(panel.m_background.color & 0x00000011) m_renderedUiItems.put<RenderedUIItems::Background>(panel.m_background);
    if(panel.m_blured) m_renderedUiItems.put<RenderedUIItems::ToBlur>(RenderedUIItems::ToBlur {panel.m_background.box, panel.m_background.depth });
}
void Styler::render(Item& item){
    m_renderedUiItems.put<RenderedUIItems::ColoredBox>({item.m_size, item.m_depth, item.m_color.value_or(0xf0f0f0f0)});
}
void Styler::renderSlider(Item& item, float ratio){
    m_renderedUiItems.put<RenderedUIItems::ColoredBox>({item.m_size, item.m_depth, 0x808080f0});
    auto s = item.m_size;
    s[2] *= ratio;
    m_renderedUiItems.put<RenderedUIItems::ColoredBox>({s, item.m_depth, 0xf0f0f0f0});

}
void Styler::renderText(Item& item, const std::string& text){
    item.m_text->formatting = item.m_formatting.value_or(Text::Left);
    item.m_text->color = item.m_textColor.value_or(0x000000ff);
    item.m_text->font = item.m_font.value_or("ui_20_bold");
    item.m_text->depth = item.m_depth;
    item.m_text->bounds = item.m_size + glm::vec4(5,5, -10, -10);
    item.m_text->renderTo(m_renderedUiItems.get<Text::Rendered>(), text);
}
