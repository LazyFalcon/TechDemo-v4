#pragma once

class Window;


class Details
{
private:
    Window& m_window;
    std::vector<std::function<void(void)>> m_toExecute;
public:
    Details(Window& window) : m_window(window){}
    void executeAtEndOfFrame();
    void drawPositionMarker(glm::vec4 position);
    void drawCurve(const std::vector<glm::vec4>& points, uint color = 0xf0f000f0);
};
