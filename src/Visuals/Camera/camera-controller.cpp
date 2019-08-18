#include "core.hpp"
#include "camera-controller.hpp"

namespace Camera
{

std::map<std::string, positionControlState> Controller::positionControlStates =
{
    {"xxxxx", {"xxxxx", &Controller::initState, &Controller::freecamPosition}},
    {"xxxxx", {"xxxxx", &Controller::initState, &Controller::pinnedPosition}},
};
std::map<std::string, rotationControlState> Controller::rotationControlStates =
{
    {"xxxxx", {"xxxxx", &Controller::initState, &Controller::global_euler}},
    {"xxxxx", {"xxxxx", &Controller::initState, &Controller::global_euler_copyUp}},
    {"xxxxx", {"xxxxx", &Controller::initState, &Controller::local_euler}},
    {"xxxxx", {"xxxxx", &Controller::initState, &Controller::local_euler_stablized}},
    {"xxxxx", {"xxxxx", &Controller::initState, &Controller::local_focused}},
    {"xxxxx", {"xxxxx", &Controller::initState, &Controller::local_focused_stabilized}},
};


glm::vec4 Controller::freecamPosition(const glm::mat4& parentTransform, ControlInput& input, float dt){
    if(input.mode == ControlInput::FreeCamMode::Around){
        // * z and x are in horizontal plane
        input.velocity += (input.move.x*Camera::right + input.move.z*Camera::at)*glm::vec4(1,1,0,0) + glm::vec4(0,0,input.move.y*.5f,0);
    }
    else {
        m_target.impulse += input.move.x*Camera::right + input.move.y*Camera::up + input.move.z*Camera::at;
    }
    origin = origin.getTarget() + input.velocity*dt;
}
glm::vec4 Controller::pinnedPosition(const glm::mat4& parentTransform, ControlInput&, float){
    return parentTransform[3];
}
glm::quat Controller::global_euler(const glm::mat4& parentTransform, ControlInput& input){
    glm::vec2 v(input.vertical*cos(-roll) - input.horizontal*sin(-roll),
                input.vertical*sin(-roll) + input.horizontal*cos(-roll));

    pitch -= (v.x * 12.f * fov)/pi;
    yaw -= (v.y * 12.f * fov)/pi;

    roll += input.roll;

    return glm::angleAxis(yaw, Z3) * glm::angleAxis(pitch, X3);
}
glm::quat Controller::global_euler_copyUp(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}
glm::quat Controller::local_euler(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}
glm::quat Controller::local_euler_stablized(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}
glm::quat Controller::local_focused(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}
glm::quat Controller::local_focused_stabilized(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}

}
