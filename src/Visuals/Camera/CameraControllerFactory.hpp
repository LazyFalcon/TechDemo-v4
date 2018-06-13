#pragma once

class Camera;
class CameraController;
class Window;

class CameraControllerFactory
{
private:
    glm::vec2& windowSize;
public:
    CameraControllerFactory(Window &window);

    template<typename Controller, typename... Args>
    std::shared_ptr<Controller> create(Args&... args){
        std::shared_ptr<Controller> out(std::make_shared<Controller>(args..., windowSize));
        // dummyInitCamera(*out);
        return out;
    }
    // void dummyInitCamera(Camera&);
};
