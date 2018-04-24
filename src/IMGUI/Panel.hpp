#pragma once
#include "ImguiCore.hpp"
#include "RenderedUI.hpp"
#include "Layouts.hpp"

class Imgui;
class Layout;

// For dynamic number of childs
// i32 is in pixels, float is relative to parent, it requires parent to be fixed size

class Panel
{
public:
    Panel(Imgui& imgui, Panel* parentPanel=nullptr) : m_imgui(imgui), m_parent(parentPanel){}
    Panel& panel(); // returns active child panel
    void operator()();

    Panel& newFixedPanel();
    Panel& newFixedPanel(int w, int h);

    // control part
    Layout& layout();

    Panel& width(float);
    Panel& width(i32);
    Panel& height(float);
    Panel& height(i32);
    Panel& x(float);
    Panel& x(i32);
    Panel& y(float);
    Panel& y(i32);

    // apperance
    Panel& fill();

    // utils
    Panel& color(u32);

private:
    Imgui& m_imgui;
    Panel* m_parent {nullptr};
    Layout m_layout;

    iBox m_size;
    iBox m_bounds;
    bool m_isFixedSize {false};

    RenderedUI::Background m_background;
};
