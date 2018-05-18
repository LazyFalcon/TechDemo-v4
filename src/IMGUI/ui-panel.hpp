#pragma once
#include "ui-core.hpp"
#include "ui-rendered.hpp"
#include "ui-layout.hpp"
#include "ui-items.hpp"

class Imgui;
class Layout;
class Styler;

// For dynamic number of childs
// i32 is in pixels, float is relative to parent, it requires parent to be fixed size

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

    Panel& width(float);
    Panel& width(i32);
    Panel& height(float);
    Panel& height(i32);
    Panel& x(float);
    Panel& x(i32);
    Panel& y(float);
    Panel& y(i32);

    i32 getRelative(i32 idx, float rel) const {
        return rel <= 1.f and rel >= -1.f ? m_size[idx] * rel : rel;
    }

    // apperance
    Panel& fill();
    Panel& color(u32);
    Panel& blured(u32);

    // create items
    Item button();
    Item item();
    Item slider();


    void reset(){
        m_childCount = 0;
    }

private:
    friend Styler;
    Imgui* m_imgui {nullptr};
    Panel* m_parent {nullptr};
    Styler* m_style {nullptr};
    Layout m_layout;

    float m_depth {};
    glm::vec4 m_size;
    iBox m_bounds;
    int m_childCount {};

    RenderedUIItems::Background m_background;
    bool m_blured {false};
};
