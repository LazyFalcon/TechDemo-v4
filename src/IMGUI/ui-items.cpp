#include "ui-items.hpp"
#include "ui-panel.hpp"
#include "ui-styler.hpp"
#include "ui.hpp"

Item& Item::x(i32 i){
    m_size[0] = i;
    return *this;
}
Item& Item::x(float i){
    m_size[0] = m_panel.getRelative(2, i);
    return *this;
}
Item& Item::y(i32 i){
    m_size[1] = i;
    return *this;
}
Item& Item::y(float i){
    m_size[1] = m_panel.getRelative(3, i);
    return *this;
}
Item& Item::w(i32 i){
    m_size[2] = i;
    return *this;
}
Item& Item::w(float i){
    m_size[2] = m_panel.getRelative(2, i);
    return *this;
}
Item& Item::h(i32 i){
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
    m_panel.getUi().getKey(m_size);

    // render -> panel ma styler który powinien wygenerować odpowiednie elementy na podstawie danych o przycisku

    m_panel.getStyler().render(*this);

    return *this;
}


Item& Item::text(const std::string& text){
    if(not m_text) m_text.emplace();

    m_panel.getStyler().renderText(*this, text);

    return *this;
}

bool Item::isDefaultPressed(){
    return m_ui.input.main.pressed(m_size);
}
bool Item::isAlternatePressed(){
    return m_ui.input.alternate.pressed(m_size);
}
