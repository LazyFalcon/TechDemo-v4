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
