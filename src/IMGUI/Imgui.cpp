#include "Imgui.hpp"


void Imgui::reset(){
    if(m_panelStack.size() != 1){
        throw InvaliUiOperation("Not all panels were finished!");
    }
    m_style.reset();
    // cleanup all states
}

Panel& Imgui::newFixedPanel(){
    m_panelStack.emplace_back(*this, &m_panelStack.back());
    return m_panelStack.back();
}
