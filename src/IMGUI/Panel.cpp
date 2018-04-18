#include "Panel.hpp"
#include "Imgui.hpp"

void Panel::operator()(){
    // m_parent->finishChildWidget(m_size);
}


Panel& Panel::newFixedPanel(){
    return m_imgui.newFixedPanel();
}

Panel& Panel::width(float w){
    return width(w * m_parent->m_size.z);
}
Panel& Panel::width(i32 w){
    m_size.z = w;
    return *this;
}
Panel& Panel::height(float h){
    return height(h * m_parent->m_size.w);
}
Panel& Panel::height(i32 h){
    m_size.w = h;
    return *this;
}
