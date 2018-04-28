#pragma once
#include "common.hpp"

class Context;
class RenderedUIItems;
class Window;

class UIRender
{
private:
    Window& m_window;
    Context& m_context;

    void depthPrepass(RenderedUIItems&);

    template<typename T>
    void render(std::vector<T>&){}

public:
    UIRender(Window& window, Context& context) : m_window(window), m_context(context){}
    void render(RenderedUIItems&);
};
