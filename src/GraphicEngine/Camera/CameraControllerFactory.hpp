#pragma once

class Camera;
class CameraController;
class Window;

class CameraControllerFactory
{
private:
    Window &window;
public:
    CameraControllerFactory(Window &window) : window(window){}

    template<typename Controller, typename... Args>
    std::shared_ptr<Controller> create(Args&... args){
        std::shared_ptr<Controller> out(std::make_shared<Controller>(args...));
        dummyInitCamera(*out);
        return out;
    }
    void dummyInitCamera(Camera&);
};
