#pragma once

#include "Waypoint.hpp"

class Context;
class Window;

class Details
{
private:
    Context& m_context;
    Window& m_window;
    std::vector<std::function<void(void)>> m_toExecute;
public:
    Details(Context& context, Window& window) : m_context(context), m_window(window){}
    void executeAtEndOfFrame();
    void drawPositionMarker(glm::vec4 position);
    void drawCurve(const std::vector<glm::vec4>& points, uint color = 0xf0f000f0);
    void drawWaypoints(const std::vector<Waypoint>& points, uint color = 0xf0f000f0);
};
