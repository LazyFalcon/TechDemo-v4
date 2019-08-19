#include "core.hpp"
#include "Logging.hpp"
#include "camera-controller.hpp"

namespace camera
{

std::list<Controller*> listOfControllers;
Controller* activeCamera;
// todo: stack of previously active cameras?

Controller& active(){
    return activeCamera;
}

Controller::Controller(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, std::string_view type, glm::vec2 windowSize):
    yaw(0),
    pitch(0, -pi/3, pi/3),
    roll(0, -pi/2, pi/2),
    fovLimited(Camera::fov, 30*toRad, 120*toRad),
    origin(parentMatrix[3], 0.1f, 0.5f),
    rotation(glm::angleAxis(0.f, Z3), 0.1f, 0.5f)
{
    listOfControllers.push_back(this);
    if(not activeCamera) focusOn();

    setBehavior(type);

    glm::extractEulerAngleXYZ(cameraRelativeMatrix, *pitch, *yaw, *roll);
    offset = calculateEyePositionOffset(cameraRelativeMatrix);
    recalculateCamera();
}

Controller::~Controller(){
    listOfControllers.remove(this);
    if(activeCamera == this and not listOfControllers.empty()) listOfControllers.front()->focus();

}

void Controller::focusOn(){
    activeCamera = this;
}
bool Controller::hasFocus() const {
    return activeCamera == this;
}

void Controller::printDebug(){
    Camera::printDebug();
    console.log("yaw, pitch, roll:", yaw*toDeg, pitch*toDeg, roll*toDeg);
}



std::map<std::string, positionControlState> Controller::positionControlStates =
{
    {"freecam", {"freecam", &Controller::initState, &Controller::freecamPosition}},
    {"pinned", {"pinned", &Controller::initState, &Controller::pinnedPosition}},
};
std::map<std::string, rotationControlState> Controller::rotationControlStates =
{
    {"global-euler", {"global-euler", &Controller::initState, &Controller::global_euler}},
    {"global-euler-copyUp", {"global-euler-copyUp", &Controller::initState, &Controller::global_euler_copyUp}},
    {"local-euler", {"local-euler", &Controller::initState, &Controller::local_euler}},
    {"local-euler-stablilized", {"local-euler-stablilized", &Controller::initState, &Controller::local_euler_stablized}},
    {"local-focused", {"local-focused", &Controller::initState, &Controller::local_focused}},
    {"local-focused-stablized", {"local-focused-stablized", &Controller::initState, &Controller::local_focused_stabilized}},
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
