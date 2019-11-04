#include "core.hpp"
#include "gl_core_4_5.hpp"
#include <GLFW/glfw3.h>
#include "App.hpp"
#include "Logger.hpp"
#include "Settings.hpp"
#include "Window.hpp"

Window::~Window(){
    console.log("~Window");
    glfwDestroyWindow(window);
}
bool Window::init(){
    size = app.settings->video.size;

    if (glfwInit() != 1){
        console.error("GLFW init fail");
        return false;
    }

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, gl::FALSE_);
    glfwWindowHint(GLFW_DECORATED, gl::FALSE_);

    screenSize.x = mode->width;
    screenSize.y = mode->height;

    if(app.settings->video.fullscreen){
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        size.x = mode->width;
        size.y = mode->height;
        // window = glfwCreateWindow(size.x, size.y, "Tech demo v4", glfwGetPrimaryMonitor(), nullptr);
        window = glfwCreateWindow(size.x, size.y, "Tech demo v4", nullptr, nullptr);
        if(!window){
            console.error("Window creation failed");
            glfwTerminate();
            return false;
        }
        glfwSetWindowPos(window, 0,0);
    }
    else {
        size.x = std::min((int)size.x, mode->width);
        size.y = std::min((int)size.y, mode->height);
        app.settings->video.size = size;

        window = glfwCreateWindow(size.x, size.y, "Tech demo v4", nullptr, nullptr);
        if(!window){
            console.error("Window creation failed");
            glfwTerminate();
            return false;
        }
        glfwSetWindowPos(window, screenSize.x/2 - size.x/2, screenSize.y/2 - size.y/2);
    }
    hide();

    // glfwSetWindowSize(window, size.x, size.y);
    // glfwSetWindowTitle(window, "Tech demo v4");
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    pixelSize = 1.f/size;
    center = size * 0.5f;
    topCenter = center;
    bottomCenter = center;
    topCenter.y = size.y;
    bottomCenter.y = 0.f;
    aspect = size.x/size.y;

    viewport = glm::vec4(0, 0, size.x, size.y);

    gl::exts::LoadTest didLoad = gl::sys::LoadFunctions();
    if(!didLoad){
        console.error("GL init fail");
        return false;
    }

    return true;
}

void Window::hide(){
    glfwHideWindow(window);
}
void Window::show(){
    glfwShowWindow(window);
}
