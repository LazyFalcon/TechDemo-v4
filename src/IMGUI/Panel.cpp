#include "Panel.hpp"
#include "Imgui.hpp"
#include "Layouts.hpp"
#include "Logging.hpp"

void Panel::operator()(){
    // m_parent->finishChildWidget(m_size);
    // tu powinien odpalić się styler, używany jako
    m_background.box = m_size;
    m_background.depth = 0;
    m_imgui.getToRender().put<RenderedUI::Background>(m_background);
    m_imgui.finishPanel(this);
}


Panel& Panel::newFixedPanel(){
    auto& p = m_imgui.newFixedPanel();
    // może niech layout ustawia tu odpowiednio, i niech ustalnaie rozmiaru odbywa się potem?
    p.m_size = m_layout.yeld({}); // now we will receive position, size have to be filler later

    return p;
}

Panel& Panel::newFixedPanel(int w, int h){
    auto& p = m_imgui.newFixedPanel();
    // może niech layout ustawia tu odpowiednio, i niech ustalnaie rozmiaru odbywa się potem?
    p.m_size = m_layout.yeld({0,0,w,h}); // now we will receive position depend on provided size

    return p;
}

Panel& Panel::width(float w){
    return width(i32(w * m_parent->m_size.z));
}
Panel& Panel::width(i32 w){
    m_size.z = w;
    return *this;
}
Panel& Panel::height(float h){
    return height(i32(h * m_parent->m_size.w));
}
Panel& Panel::height(i32 h){
    m_size.w = h;
    return *this;
}
Panel& Panel::x(float p){
    return x(i32(p * m_parent->m_size.z));
}
Panel& Panel::x(i32 p){
    m_size.x = p + m_parent->m_size.x;
    return *this;
}
Panel& Panel::y(float p){
    return y(i32(p * m_parent->m_size.w));
}
Panel& Panel::y(i32 p){
    m_size.y = p + m_parent->m_size.y;
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
