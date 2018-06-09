#pragma once
#include <vector>

class RenderedUIItems;
class Panel;
class Item;

class Styler
{
private:
    RenderedUIItems& m_renderedUiItems;

public:
    Styler(RenderedUIItems& renderedUiContainer) : m_renderedUiItems(renderedUiContainer) {}
    void render(Panel& panel);
    void render(Item& item);
    void renderSlider(Item& item, float ratio);
    void renderText(Item& item, const std::string& text);
    void renderSymbol(Item& item, const std::u16string& text, const std::string& font = "sym_20");
};
