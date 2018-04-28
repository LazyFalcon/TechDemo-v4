#pragma once
#include "common.hpp"
// TODO: rename this, pleaseeee

class Panel;
class Styler;

class Item
{
public:
    Item(Panel& p) : m_panel(p){}
    // positioning
    Item& x(i32 i);
    Item& x(float i);
    Item& y(i32 i);
    Item& y(float i);
    Item& w(i32 i);
    Item& w(float i);
    Item& h(i32 i);
    Item& h(float i);

    Item& operator()();

    //
    Item& text(const std::string& text);

private:
    friend Styler;
    glm::vec4 m_size{};
    Panel& m_panel;
    std::optional<i32> m_keyPressed;
};
