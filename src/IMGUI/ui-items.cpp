#include "ui-items.hpp"
#include "ui-panel.hpp"
#include "ui-styler.hpp"
#include "ui.hpp"

Item& Item::x(int i){
    m_size[0] = i;
    return *this;
}
Item& Item::x(float i){
    m_size[0] = m_panel.getRelative(2, i);
    return *this;
}
Item& Item::y(int i){
    m_size[1] = i;
    return *this;
}
Item& Item::y(float i){
    m_size[1] = m_panel.getRelative(3, i);
    return *this;
}
Item& Item::w(int i){
    m_size[2] = i;
    return *this;
}
Item& Item::w(float i){
    m_size[2] = m_panel.getRelative(2, i);
    return *this;
}
Item& Item::h(int i){
    m_size[3] = i;
    return *this;
}
Item& Item::h(float i){
    m_size[3] = m_panel.getRelative(3, i);
    return *this;
}

Item& Item::operator()(){
    // odpalić feedback do panelu -> przekazać mu swój rozmiar i pozycję, uzyskać od niego poprawione wartości,
    // -> między innymi kwadrat wyrównany do odpowiedniego kierunku, zaaplikowany padding, narzucone wymiary
    m_size = m_panel.getLayout().feedback(m_size);
    // następnie feedback od ui -> akcje od myszy które odbywają się na tym prostokącie, pamiętać że mogą być różne typy przycisków
    // defaultowe akcje w zależności o typu
    m_action = m_ui.getPointerAction(m_size, m_depth);
    m_hovered = m_action>=PointerActions::Hover and m_action<=PointerActions::RmbHold;
    // render -> panel ma styler który powinien wygenerować odpowiednie elementy na podstawie danych o przycisku

    m_panel.getStyler().render(*this);

    return *this;
}

Item& Item::operator()(float& value, float min, float max){
    m_size = m_panel.getLayout().feedback(m_size);
    m_action = m_ui.getPointerAction(m_size, m_depth);
    m_hovered = m_action>=PointerActions::Hover and m_action<=PointerActions::RmbHold;

    float ratio = (value - min)/(max - min);
    float slidePosition = m_size.x + m_size[2] * ratio;

    m_panel.getStyler().renderSlider(*this, ratio);

    // now slider behavior
    // release slider
    if(m_ui.editedValue == &value and m_ui.input.lmb.off){
       m_ui.editedValue = nullptr;
    }
    // Move slider by mouse translation
    else if(m_ui.editedValue == &value and m_ui.input.lmb.position){
        value += m_ui.input.mouseTranslation.x/m_size[2] * (max - min);
        value = glm::clamp(value, min, max);
    }
    // Catching slider
    bool isSlideHovered = m_ui.input.hover(glm::vec4(slidePosition-7, m_size.y, 14, m_size.w), m_depth);
    if(isSlideHovered and m_ui.input.lmb.on){
        m_ui.editedValue = &value;
    }
    // if was pressed outside slide, move value in direction by 10%
    else if(m_hovered and m_ui.input.lmb.on){
        value += glm::sign(m_ui.input.mousePos.x - slidePosition) * 0.1f * (max - min);
        value = glm::clamp(value, min, max);
    }
    // else if(m_hovered and m_ui.input.scroll){
    //     value += glm::sign(m_ui.input.mousePos.x - slidePosition) * 0.1f * (max - min);
    //     value = glm::clamp(value, min, max);
    // }

    return *this;
}

Item& Item::text(const std::string& text){
    if(not m_text) m_text.emplace();

    m_panel.getStyler().renderText(*this, text);

    return *this;
}

bool Item::isLmbPressed(){
    return m_ui.input.lmb.pressedOff(m_size, m_depth);
}
bool Item::isRmbPressed(){
    return m_ui.input.rmb.pressedOn(m_size, m_depth);
}
bool Item::isHover(){
    return m_ui.input.hover(m_size, m_depth);
}
bool Item::isAnyAction(){
    return m_ui.input.rmb.on or m_ui.input.rmb.off or m_ui.input.lmb.on or m_ui.input.lmb.off;
}
