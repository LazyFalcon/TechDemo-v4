#pragma once
#include "ui-core.hpp"
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
    void renderText(Item& item, const std::string& text);
};
