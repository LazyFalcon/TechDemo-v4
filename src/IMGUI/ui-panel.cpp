#include "ui-panel.hpp"
#include "ui.hpp"
#include "Logging.hpp"

void Panel::operator()(){
    // m_parent->finishChildWidget(m_size);
    // tu powinien odpalić się styler, używany jako
    m_background.box = m_size;
    m_background.depth = 0;
    m_style->render(*this);
    m_imgui.finishPanel(this);
}


Panel& Panel::newFixedPanel(){
    auto& p = m_imgui.newFixedPanel();
    // może niech layout ustawia tu odpowiednio, i niech ustalnaie rozmiaru odbywa się potem?
    p.m_size = m_layout.calcPosition({}); // now we will receive position, size have to be filler later,\
     no idea how it will be calculater, but doesn't care

    return p;
}

Panel& Panel::newFixedPanel(int w, int h){
    auto& p = m_imgui.newFixedPanel();
    // może niech layout ustawia tu odpowiednio, i niech ustalnaie rozmiaru odbywa się potem?
    p.m_size = m_layout.calcPosition({0,0,w,h}); // now we will receive position depend on provided size

    return p;
}

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


Item Panel::button(){
    Item i(m_imgui, *this);

    return i;
}
