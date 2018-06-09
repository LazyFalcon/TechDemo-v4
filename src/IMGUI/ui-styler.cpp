#include "core.hpp"
#include "ui-core.hpp"
#include "ui-styler.hpp"
#include "ui-panel.hpp"
#include "ui-items.hpp"
#include "ui-rendered.hpp"


void Styler::render(Panel& panel){
    // panel.m_background.color = 0x10101010;
    if(panel.m_background.color) m_renderedUiItems.put<RenderedUIItems::Background>(panel.m_background);
    if(panel.m_blured) m_renderedUiItems.put<RenderedUIItems::ToBlur>(RenderedUIItems::ToBlur {panel.m_background.box, panel.m_background.depth });
}
void Styler::render(Item& item){
    u32 itemColor = 0xf0f0f040;
    if(item.m_action > PointerActions::Hover and item.m_action <= PointerActions::RmbHold) itemColor = 0xf0f0f0a0;
    else if(item.m_action == PointerActions::Hover) itemColor = 0xf0f0f080;

    itemColor = item.m_color.value_or(itemColor);

    if(itemColor) m_renderedUiItems.put<RenderedUIItems::ColoredBox>({item.m_size, item.m_depth, itemColor});
}
void Styler::renderSlider(Item& item, float ratio){
    auto size = item.m_size;

    float heightCorrection = size[3] * 0.8f;
    size[1] += heightCorrection;
    size[3] -= 2.f*heightCorrection;
    m_renderedUiItems.put<RenderedUIItems::ColoredBox>({size, item.m_depth, 0xa0a0a050});
    size[2] *= ratio;
    m_renderedUiItems.put<RenderedUIItems::ColoredBox>({size, item.m_depth, 0xf0f0f0f0});

}
void Styler::renderText(Item& item, const std::string& text){
    item.m_text->formatting = item.m_formatting.value_or(Text::Left);
    item.m_text->color = item.m_textColor.value_or(0xf0f0f0d0);
    item.m_text->font = item.m_font.value_or("ui_20");
    item.m_text->depth = item.m_depth;
    item.m_text->bounds = item.m_size + glm::vec4(5,5, -10, -10);
    item.m_text->renderTo(m_renderedUiItems.get<Text::Rendered>(), text);
}
void Styler::renderSymbol(Item& item, const std::u16string& text, const std::string& font){
    Text textItem;
    textItem.formatting = item.m_formatting.value_or(Text::Left);
    textItem.color = item.m_textColor.value_or(0xf0f0f0d0);
    textItem.font = font;
    textItem.depth = item.m_depth;
    textItem.bounds = item.m_size + glm::vec4(5,5, -10, -10);
    textItem.renderTo(m_renderedUiItems.get<Text::Rendered>(), text);
}
