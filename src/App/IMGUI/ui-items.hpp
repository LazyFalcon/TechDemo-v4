#pragma once
#include "ui-core.hpp"
#include "ui-text.hpp"

class Imgui;
class Panel;
class Styler;

class Item
{
public:
    enum ItemType
    {
        Button,
        Slider,
        TextOnly,
        Radio,
        Checkbox
    };

    Item(ItemType i, Imgui& ui, Panel& p, float depth, int id)
        : m_type(i), m_ui(ui), m_panel(p), m_depth(depth), m_id(id) {}
    // positioning
    Item& x(int i);
    Item& x(float i);
    Item& y(int i);
    Item& y(float i);
    Item& w(int i);
    Item& w(float i);
    Item& h(int i);
    Item& h(float i);

    // finishing functions
    // injects Item to panel, collects input, calculates bounding box and renders item
    Item& operator()();
    // for slider item
    Item& operator()(float& value, float min, float max);

    Item& font(const std::string& f) {
        if(not f.empty())
            m_font = f;
        return *this;
    }
    Item& textColor(u32 c) {
        m_textColor = c;
        return *this;
    }
    Item& color(u32 c) {
        m_color = c;
        return *this;
    }
    Item& formatting(Text::Formatting f) {
        m_formatting = f;
        return *this;
    }
    Item& text(const std::string& text);
    Item& symbol(const std::u16string& text, const std::string& font = "sym_20");

    int id() const {
        return m_id;
    }
    bool isLmbPressed();
    bool isRmbPressed();
    bool isAnyAction();
    bool isHover(); // au
    // default for LLM actions, unfortunately default is restricted keyword
    template<typename Callback>
    Item& action(Callback&& c) {
        if(m_action == PointerActions::LmbOff)
            c();
        return *this;
    }
    template<typename Callback>
    Item& actionOutside(Callback&& c) {
        if(m_action == PointerActions::ActionOutside)
            c();
        return *this;
    }
    template<typename Callback>
    Item& alternate(Callback&& c) {
        if(m_action == PointerActions::RmbOff)
            c();
        return *this;
    }
    template<typename Callback>
    Item& hover(Callback&& c) {
        if(m_hovered)
            c();
        return *this;
    }

    Item& switchBool(bool& b) {
        if(m_action == PointerActions::LmbOff)
            b = !b;
        return *this;
    }

private:
    friend Styler;
    ItemType m_type;
    glm::vec4 m_size {};
    Imgui& m_ui;
    Panel& m_panel;

    float m_depth;
    int m_id;
    PointerActions m_action {PointerActions::None};
    bool m_hovered;

    std::optional<int> m_keyPressed;
    std::optional<int> m_image;

    std::optional<std::string> m_font;
    std::optional<u32> m_textColor;
    std::optional<u32> m_color;
    std::optional<Text::Formatting> m_formatting;
    std::optional<Text> m_text;
};
