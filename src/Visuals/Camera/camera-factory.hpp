#pragma once

class Window;

namespace camera {

class Camera;
class Controller;

class Factory
{
private:
    glm::vec2& windowSize;
public:
    Factory(Window &window);

    template<typename ControllerType, typename... Args>
    std::shared_ptr<ControllerType> create(Args&&... args){
        auto out(std::make_shared<ControllerType>(std::forward<Args>(args)..., windowSize));
        // dummyInitCamera(*out);
        return out;
    }
};
}
