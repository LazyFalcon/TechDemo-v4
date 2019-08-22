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

void Controller::update(const glm::mat4& parentTransform, float dt){
    if(not hasFocus()) return;

    zoom();

    rotation = computeTargetRotation(parentTransform, dt);
    origin = computeTargetPosition(parentTransform, dt);

    rotation.update(dt);
    origin.update(dt);

    Camera::orientation = glm::toMat4(computeRotation(parentTransform, dt));
    Camera::orientation[3] = origin.get() + Camera::orientation * offset*offsetScale;
    Camera::recalculate();

    alterTargetRotation();

    handleInput(parentTransform, dt);
    updateMovement(parentTransform, dt);
    recalculateCamera();
}

void Controller::zoom(){
    if(zoomDirection == 0.f) return;

    if(zoomByFov){
        fov = fov + zoomDirection*15.f;
    }
    else {
        offsetScale += zoomDirection*2;
    }
}

glm::quat Controller::computeTargetRotation(const glm::mat4& parentTransform, float dt){
    if(worldPointToFocusOn or worldPointToFocusOnWhenSteady){
        const auto dir = glm::normalize((*worldPointToFocusOn - eyePosition()).xyz());
        return glm::angleAxis(acos(glm::dot(dir, Z3)), glm::normalize(glm::cross(dir, Z3)));
         // albo oszczędzając acos:
         // float s = sqrt( (1+glm::dot(dir, Z3))*2 );
         // float invs = 1 / s;

         // return glm::quat(
         //     s * 0.5f,
         //     rotationAxis.x * invs,
         //     rotationAxis.y * invs,
         //     rotationAxis.z * invs
         // );
    }

    // niestety na razie kąty eulera
    glm::vec2 v(pointerMovement.vertical*cos(-roll) - pointerMovement.horizontal*sin(-roll),
                pointerMovement.vertical*sin(-roll) + pointerMovement.horizontal*cos(-roll));

    pitch -= (v.x * 12.f * fov)/pi;
    yaw -= (v.y * 12.f * fov)/pi;

    roll += pointerMovement.roll;

    return glm::angleAxis(yaw, Z3) * glm::angleAxis(pitch, X3);
}

glm::vec4 Controller::computeTargetPosition(const glm::mat4& parentTransform, float dt){
    if(not freecam) return parentTransform[3];

    if(moveHorizontally){
        return origin.get() +  (directionOfMovement.x*right + directionOfMovement.z*at)*glm::vec4(1,1,0,0) + glm::vec4(0,0,directionOfMovement.y*.5f,0);
    }
    else {
        return origin.get() + directionOfMovement.x*right + directionOfMovement.y*up + directionOfMovement.z*at;
    }
}

glm::quat Controller::getRotationBasis(const glm::mat4& parentTransform, float dt){
    // modify current rotation
    if(not parentRotationAffectCurrentRotation) return rotation.get();

    auto parentQuat = glm::quat_cast(parentTransform);

    if(not smoothParentRotation) parentQuat*rotation.get();
    return glm::slerp(rotation.get(), parentQuat*rotation.get(), 0.5f)
    // co powinno się tu stać?
    // 1. Zmieniamy układ odniesienia kamery - obraca się ona razem z rodzicem - trzeba zmodyfikować obecne camera::orientation
}

void Controller::alterTargetRotation(){
    if(not keepRightAxisHorizontal) return;

    // sprawdzić kierunki!
    const auto axis = glm::normalize(glm::cross(Z3, right.xyz()));
    const auto angle = pi/2 - acos(glm::dot(Z3, right.xyz()));
    if(abs(angle) < 0.0001f) return; // too close
    target = target * glm::angleAxis(angle, axis);
}

}
