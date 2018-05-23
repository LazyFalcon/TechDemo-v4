#pragma once
#include "ui-core.hpp"
#include "ui-rendered.hpp"
#include "ui-layout.hpp"
#include "ui-items.hpp"

class Imgui;
class Layout;
class Styler;

// For dynamic number of childs
// int is in pixels, float is relative to parent, it requires parent to be fixed size

class Panel
{
public:
    Panel(Imgui* imgui, Styler* style, Panel* parentPanel=nullptr);
    Panel(Panel&); // create new panel attached to Panel
    Panel(Imgui&); // create free panel, attached to default ui panel
    Panel& operator=(const Panel&) = delete;
    Panel& panel(); // returns active child panel
    Panel& operator()(); // finish panel styling and positioning

    // control part
    Layout& layout();
    Layout& getLayout(){
        return m_layout;
    }

    Imgui& getUi(){
        return *m_imgui;
    }
    Styler& getStyler(){
        return *m_style;
    }
    const glm::vec4& getSize(){
        return m_size;
    }

    Panel& width(float);
    Panel& width(int);
    Panel& height(float);
    Panel& height(int);
    Panel& x(float);
    Panel& x(int);
    Panel& y(float);
    Panel& y(int);

    // misc
    int getRelative(int idx, float rel) const {
        return rel <= 1.f and rel >= -1.f ? m_size[idx] * rel : rel;
    }

    template<typename Callback>
    Panel& action(Callback&& c){
        if(m_action == PointerActions::LmbOff) c();
        return *this;
    }
    template<typename Callback>
    Panel& actionOutside(Callback&& c){
        if(m_action == PointerActions::ActionOutside) c();
        return *this;
    }
    template<typename Callback>
    Panel& alternate(Callback&& c){
        if(m_action == PointerActions::RmbOff) c();
        return *this;
    }
    template<typename Callback>
    Panel& hover(Callback&& c){
        if(m_hovered) c();
        return *this;
    }

    template<typename Callback>
    Panel& quickStyler(Callback&& c){
        m_quickStyler = std::move(c);
        return *this;
    }

    bool onKey(const std::string& key);

    // apperance
    Panel& fill();
    Panel& color(u32);
    Panel& blured(u32);

    // create items
    Item button();
    Item item();
    Item slider();
    Item checkbox();
    Item radio();

    void reset(){
        m_childCount = 0;
    }

private:
    friend Styler;
    Imgui* m_imgui {nullptr};
    Panel* m_parent {nullptr};
    Styler* m_style {nullptr};
    Layout m_layout;
    std::function<void(Item&)> m_quickStyler;

    int m_itemId{};
    float m_depth {};
    glm::vec4 m_size;
    glm::vec4 m_bounds;
    int m_childCount {};
    PointerActions m_action {PointerActions::None};

    // * when mouse is clearly over panel, doesn't count items
    bool m_hovered;

    RenderedUIItems::Background m_background;
    bool m_blured {false};
};
