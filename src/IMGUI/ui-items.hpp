#pragma once
#include "common.hpp"
#include "ui-text.hpp"
// TODO: rename this, pleaseeee

class Imgui;
class Panel;
class Styler;

class Item
{
public:
    Item(Imgui& ui, Panel& p) : m_ui(ui), m_panel(p){}
    // positioning
    Item& x(i32 i);
    Item& x(float i);
    Item& y(i32 i);
    Item& y(float i);
    Item& w(i32 i);
    Item& w(float i);
    Item& h(i32 i);
    Item& h(float i);

    // injects Utrm to panel, collects input, calculates bounding box and renders item
    Item& operator()();

    //
    Item& font(const std::string& f){
        m_font = f;
        return *this;
    }
    Item& textColor(u32 c){
        m_textColor = c;
        return *this;
    }
    Item& formatting(Text::Formatting f){
        m_formatting = f;
        return *this;
    }
    Item& text(const std::string& text);

    bool isDefaultPressed();
    bool isAlternatePressed();
    // default for LLM actions, unfortunately default is restricted keyword
    template<typename Callback>
    Item& action(Callback&& c){
        if(isDefaultPressed()) c();
        return *this;
    }

private:
    friend Styler;
    glm::vec4 m_size{};
    Imgui& m_ui;
    Panel& m_panel;
    std::optional<i32> m_keyPressed;
    std::optional<i32> m_image;

    std::optional<std::string> m_font;
    std::optional<u32> m_textColor;
    std::optional<Text::Formatting> m_formatting;
    std::optional<Text> m_text;
};
