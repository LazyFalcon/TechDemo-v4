#pragma once
#include "camera-control-policy.hpp"

namespace Camera
{

class GlobalEuler : public ControlPolicy
{
private:
    Utils::Limits<float, periodicAngle<float>> yaw; // y, around Z axis
    Utils::Limits<float> pitch; // x, around X axis
    Utils::Limits<float> roll; // z, around Y axis

public:
    GlobalEuler() :
        yaw(0),
        pitch(0, -pi/3, pi/3),
        roll(0, -pi/2, pi/2)
    {}
    update(ControlInput& input, float dt){


    }
    glm::quat calculate(ControlInput&, const glm::mat4&){
        glm::vec2 v(input.vertical*cos(-roll) - input.horizontal*sin(-roll),
                    input.vertical*sin(-roll) + input.horizontal*cos(-roll));

        pitch -= (v.x * 12.f * fov)/pi;
        yaw -= (v.y * 12.f * fov)/pi;


    }
};


};
