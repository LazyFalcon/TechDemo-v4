#pragma once
#include "ImguiCore.hpp"

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

    // control part
    Panel& layout(Layout&);

    Panel& width(float);
    Panel& width(i32);
    Panel& height(float);
    Panel& height(i32);

private:
    Imgui& m_imgui;
    Panel* m_parent {nullptr};
    Layout* m_layout {nullptr};

    iBox m_size;
    bool m_isFixedSize {false};
};
