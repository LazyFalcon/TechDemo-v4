#include "ui-panel.hpp"
#include "ui.hpp"
#include "Logging.hpp"

Panel& Panel::operator()(){
    // feedback size to parent layout
    if(m_parent) m_size = m_parent->m_layout.feedback(m_size);

    m_background.box = m_size;
    m_background.depth = m_depth;
    m_imgui->input.hover(m_size, m_depth);
    m_style->render(*this);

    return *this;
}
Panel::Panel(Imgui* imgui, Styler* style, Panel* parentPanel) : m_imgui(imgui), m_style(style),  m_parent(parentPanel){}
Panel::Panel(Panel& parent) : Panel(parent.m_imgui, &parent.getStyler(), &parent){
    parent.m_childCount++;
    m_depth = m_depth + 0.1f + 0.001f*m_childCount;
    m_size = {};
}
Panel::Panel(Imgui& imgui) : Panel(imgui.panel()){}

Panel& Panel::width(float w){
    return width(m_parent->getRelative(2, w));
}
Panel& Panel::width(i32 w){
    m_size.z = w;
    return *this;
}
Panel& Panel::height(float h){
    return height(m_parent->getRelative(3, h));
}
Panel& Panel::height(i32 h){
    m_size.w = h;
    return *this;
}
Panel& Panel::x(float p){
    return x(m_parent->getRelative(2, p));
}
Panel& Panel::x(i32 p){
    if(p < 0) m_size.x = m_parent->m_size.x + m_parent->m_size.z + p;
    else m_size.x = p + m_parent->m_size.x;

    return *this;
}
Panel& Panel::y(float p){
    return y(m_parent->getRelative(3, p));
}
Panel& Panel::y(i32 p){
    if(p < 0) m_size.y = m_parent->m_size.y + m_parent->m_size.w + p;
    else m_size.y = p + m_parent->m_size.y;
    return *this;
}

Layout& Panel::layout(){
    m_layout.setBounds(m_size);
    m_layout.compile();
    return m_layout;
}


Panel& Panel::fill(){
    return *this;
}

Panel& Panel::color(u32 c){
    m_background = {};
    m_background.color = c;
    return *this;
}
Panel& Panel::blured(u32 c){
    m_background = {};
    m_background.color = c;
    m_blured = true;
    return *this;
}


Item Panel::button(){
    Item i(Item::Button, *m_imgui, *this, m_depth+0.0001f);

    return i;
}

Item Panel::slider(){
    Item i(Item::Slider, *m_imgui, *this, m_depth+0.0001f);

    return i;
}
