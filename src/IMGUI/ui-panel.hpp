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
    Panel(Imgui& imgui, Styler* style, Panel* parentPanel=nullptr) : m_imgui(imgui), m_style(style), m_parent(parentPanel){}
    Panel& panel(); // returns active child panel
    void operator()();

    Panel& newFixedPanel();
    Panel& newFixedPanel(int w, int h);

    // control part
    Layout& layout();
    Layout& getLayout(){
        return m_layout;
    }

    Imgui& getUi(){
        return m_imgui;
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

    i32 getRelative(i32 idx, float rel){
        return m_size[idx] * rel;
    }

    // apperance
    Panel& fill();

    // utils
    Panel& color(u32);

    // create items
    Item button();

private:
    friend Styler;
    Imgui& m_imgui;
    Panel* m_parent {nullptr};
    Styler* m_style {nullptr};
    Layout m_layout;

    glm::vec4 m_size;
    iBox m_bounds;

    RenderedUIItems::Background m_background;
};
