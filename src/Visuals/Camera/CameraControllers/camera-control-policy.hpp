#pragma once

namespace Camera
{

class ControlInput;

class ControlPolicy
{
public:
    virtual ~ControlPolicy() = default;
    virtual update(ControlInput&, float dt) = 0;
    virtual glm::quat calculate(ControlInput&, const glm::mat4&) = 0;
}

}
