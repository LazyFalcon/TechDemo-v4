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
    enum ItemType {
        Button, Slider, TextOnly
    };

    Item(ItemType i, Imgui& ui, Panel& p, float depth) : m_type(i), m_ui(ui), m_panel(p), m_depth(depth){}
    // positioning
    Item& x(i32 i);
    Item& x(float i);
    Item& y(i32 i);
    Item& y(float i);
    Item& w(i32 i);
    Item& w(float i);
    Item& h(i32 i);
    Item& h(float i);

    // finishing functions
    // injects Item to panel, collects input, calculates bounding box and renders item
    Item& operator()();
    // for slider item
    Item& operator()(float& value, float min, float max);

    //
    Item& font(const std::string& f){
        m_font = f;
        return *this;
    }
    Item& textColor(u32 c){
        m_textColor = c;
        return *this;
    }
    Item& color(u32 c){
        m_color = c;
        return *this;
    }
    Item& formatting(Text::Formatting f){
        m_formatting = f;
        return *this;
    }
    Item& text(const std::string& text);

    bool isDefaultPressed();
    bool isAlternatePressed();
    bool isHover(); // au
    // default for LLM actions, unfortunately default is restricted keyword
    template<typename Callback>
    Item& action(Callback&& c){
        if(m_hovered and isDefaultPressed()) c();
        return *this;
    }
    template<typename Callback>
    Item& alternate(Callback&& c){
        if(m_hovered and isAlternatePressed()) c();
        return *this;
    }
    template<typename Callback>
    Item& hover(Callback&& c){
        if(m_hovered and isHover()) c();
        return *this;
    }

private:
    friend Styler;
    ItemType m_type;
    glm::vec4 m_size{};
    Imgui& m_ui;
    Panel& m_panel;

    float m_depth;
    bool m_hovered;

    std::optional<i32> m_keyPressed;
    std::optional<i32> m_image;

    std::optional<std::string> m_font;
    std::optional<u32> m_textColor;
    std::optional<u32> m_color;
    std::optional<Text::Formatting> m_formatting;
    std::optional<Text> m_text;
};
