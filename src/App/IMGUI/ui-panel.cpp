#include "core.hpp"
#include "ui-panel.hpp"
#include "Logger.hpp"
#include "ui.hpp"

Panel& Panel::operator()() {
    // feedback size to parent layout
    if(m_parent)
        m_size = m_parent->m_layout.feedback(m_size);

    m_background.box = m_size;
    m_background.depth = m_depth;

    m_imgui->input.hover(m_size, m_depth); // to hide items below this panel
    m_action = m_imgui->panelInput.getPointerAction(m_size, m_depth);
    m_hovered = m_action >= PointerActions::Hover and m_action <= PointerActions::RmbHold;

    m_style->render(*this);

    return *this;
}
Panel::Panel(Imgui* imgui, Styler* style, Panel* parentPanel) : m_imgui(imgui), m_parent(parentPanel), m_style(style) {}
Panel::Panel(Panel& parent) : Panel(parent.m_imgui, &parent.getStyler(), &parent) {
    parent.m_childCount++;
    m_depth = m_depth + 0.1f + 0.001f * m_childCount;
    m_size = {};
    width(1.f);
    height(1.f);
}
Panel::Panel(Imgui& imgui) : Panel(imgui.panel()) {}

Panel& Panel::width(float w) {
    return width(m_parent->getRelative(2, w));
}
Panel& Panel::width(int w) {
    m_size.z = w;
    return *this;
}
Panel& Panel::height(float h) {
    return height(m_parent->getRelative(3, h));
}
Panel& Panel::height(int h) {
    m_size.w = h;
    return *this;
}
Panel& Panel::x(float p) {
    return x(m_parent->getRelative(2, p));
}
Panel& Panel::x(int p) {
    if(p < 0)
        m_size.x = m_parent->m_size.x + m_parent->m_size.z + p;
    else
        m_size.x = p + m_parent->m_size.x;

    return *this;
}
Panel& Panel::y(float p) {
    return y(m_parent->getRelative(3, p));
}
Panel& Panel::y(int p) {
    if(p < 0)
        m_size.y = m_parent->m_size.y + m_parent->m_size.w + p;
    else
        m_size.y = p + m_parent->m_size.y;
    return *this;
}

Layout& Panel::layout() {
    m_layout.setBounds(m_size);
    m_layout.compile();
    return m_layout;
}

Panel& Panel::fill() {
    return *this;
}

Panel& Panel::color(u32 c) {
    m_background = {};
    m_background.color = c;
    return *this;
}
Panel& Panel::blured(u32 c) {
    m_background = {};
    m_background.color = c;
    m_blured = true;
    return *this;
}

bool Panel::onKey(const std::string& key) {
    return m_hovered and key == m_imgui->pressedKey;
}

Item Panel::button() {
    Item i(Item::Button, *m_imgui, *this, m_depth + 0.0001f, m_itemId++);
    if(m_quickStyler)
        m_quickStyler(i);
    return i;
}

Item Panel::item() {
    Item i(Item::Button, *m_imgui, *this, m_depth + 0.0001f, m_itemId++);
    if(m_quickStyler)
        m_quickStyler(i);
    return i;
}

Item Panel::slider() {
    Item i(Item::Slider, *m_imgui, *this, m_depth + 0.0001f, m_itemId++);
    return i;
}

Item Panel::checkbox() {
    Item i(Item::Slider, *m_imgui, *this, m_depth + 0.0001f, m_itemId++);
    return i;
}

Item Panel::radio() {
    Item i(Item::Slider, *m_imgui, *this, m_depth + 0.0001f, m_itemId++);
    return i;
}
