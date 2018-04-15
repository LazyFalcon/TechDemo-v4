#pragma once
#include "Includes.hpp"
// #include "CameraControllerFactory.hpp"

class GLFWwindow;
class App;

class Window
{
public:
    bool init();
    void hide();
    void show();

    Window(App &app) : app(app) {}
    // Window() : camFactory(*this){}
    ~Window();

    glm::vec2 size;
    glm::vec2 screenSize;
    glm::vec2 center;
    glm::vec2 topCenter;
    glm::vec2 bottomCenter;
    glm::vec2 pixelSize;
    glm::vec4 viewport;
    float aspect;
    GLFWwindow *window;
    App &app;
    // CameraControllerFactory camFactory;
};
