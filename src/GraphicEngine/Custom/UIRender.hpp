#pragma once
#include "Includes.hpp"

class Context;
class RenderedUI;
class Window;

class UIRender
{
private:
    Window& m_window;
    Context& m_context;

    void depthPrepass(RenderedUI&);

    template<typename T>
    void render(std::vector<T>&){}

public:
    UIRender(Window& window, Context& context) : m_window(window), m_context(context){}
    void render(RenderedUI&);
};
