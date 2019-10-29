#include "core.hpp"
#include "camera-controller.hpp"
#include "Logger.hpp"

namespace camera
{
std::list<Controller*> listOfControllers;
Controller* activeCamera;
// todo: stack of previously active cameras?

Controller& active() {
    return *activeCamera;
}

bool hasActive() {
    return activeCamera != nullptr;
}

Controller::Controller(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize)
    : yaw(0),
      pitch(0),
      roll(0),
      // pitch(0, -pi/3, pi/3),
      // roll(0, -pi/2, pi/2),
      // fovLimited(Camera::fov, 30*toRad, 120*toRad),
      origin(parentMatrix[3], 0.1f, 0.5f),
      rotation(glm::quat_cast(parentMatrix * cameraRelativeMatrix), 0.1f, 0.5f),
      fovChange(Utils::Limits<float>(85.f, 20.f, 90.f), 0.1f, 0.6f),
      offsetChange(Utils::Limits<float>(85.f, 0.f, 20.f), 0.1f, 0.6f) {
    listOfControllers.push_back(this);
    if(not activeCamera)
        focusOn();

    Camera::aspectRatio = windowSize.x / windowSize.y;
    Camera::nearDistance = 0.10f;
    Camera::farDistance = 1500.f;
    Camera::fov = 85 * toRad;
    Camera::inertia = 1;
    Camera::smoothing = 1;

    console.log("Parent position:", parentMatrix[3]);

    glm::extractEulerAngleXYZ(cameraRelativeMatrix, *pitch, *yaw, *roll);

    offset = calculateEyePositionOffset(cameraRelativeMatrix);
    offsetScale = glm::length(offset) * glm::sign(glm::dot(at, offset));
    offset = glm::normalize(offset);

    Camera::orientation = glm::toMat4(glm::angleAxis(*yaw, Z3) * glm::angleAxis(*pitch, X3));
    Camera::orientation[3] = origin.get() + Camera::orientation * offset * offsetScale;
    Camera::recalculate();

    printDebug();
}

Controller::Controller(const glm::mat4& cameraWorldMatrix, glm::vec2 windowSize)
    : yaw(0),
      pitch(0),
      roll(0),
      // pitch(0, -pi/3, pi/3),
      // roll(0, -pi/2, pi/2),
      // fovLimited(Camera::fov, 30*toRad, 120*toRad),
      origin(cameraWorldMatrix[3], 0.1f, 0.5f),
      rotation(glm::quat_cast(cameraWorldMatrix), 0.1f, 0.5f),
      fovChange(Utils::Limits<float>(85.f, 20.f, 90.f), 0.1f, 0.6f),
      offsetChange(Utils::Limits<float>(85.f, 0.f, 20.f), 0.1f, 0.6f) {
    listOfControllers.push_back(this);
    if(not activeCamera)
        focusOn();

    Camera::aspectRatio = windowSize.x / windowSize.y;
    Camera::nearDistance = 0.10f;
    Camera::farDistance = 1500.f;
    Camera::fov = 85 * toRad;
    Camera::inertia = 1;
    Camera::smoothing = 1;

    console.log("New camera position:", cameraWorldMatrix[3]);

    glm::extractEulerAngleXYZ(cameraWorldMatrix, *pitch, *yaw, *roll);

    offset = glm::vec4(0, 0, -1, 0);
    offsetScale = 0;
    offset = glm::normalize(offset);

    Camera::orientation = glm::toMat4(glm::angleAxis(*yaw, Z3) * glm::angleAxis(*pitch, X3));
    Camera::orientation[3] = origin.get() + Camera::orientation * offset * offsetScale;
    Camera::recalculate();

    printDebug();
}

Controller::~Controller() {
    listOfControllers.remove(this);
    if(activeCamera == this and not listOfControllers.empty())
        listOfControllers.front()->focusOn();
}

void Controller::focusOn() {
    if(activeCamera)
        activeCamera->freeControl();
    activeCamera = this;
}

bool Controller::hasFocus() const {
    return activeCamera == this;
}

void Controller::printDebug() {
    Camera::printDebug();
    console.log("yaw, pitch, roll:", *yaw * toDeg, *pitch * toDeg, *roll * toDeg);
    console.log("setup.isFreecam:", setup.isFreecam);
    console.log("setup.zoomMode:", setup.zoomMode);
    console.log("setup.inLocalSpace:", setup.inLocalSpace);
    console.log("setup.addRotationToTarget:", setup.addRotationToTarget);
    console.log("setup.addInclinationToTarget:", setup.addInclinationToTarget);
    console.log("pointerVisible:", pointerVisible);
    console.log("input.velocity:", input.velocity);
    console.log("input.worldPointToFocusOn:", input.worldPointToFocusOn.value_or(glm::vec4(0)));
}

glm::vec4 Controller::calculateEyePositionOffset(const glm::mat4& cameraRelativeMatrix) const {
    // matrix describes camera relative position in space of module, so now we need to inverse camera matrix to get distance of module origin on each camera axis
    auto inv = glm::affineInverse(cameraRelativeMatrix);
    return inv[3] - glm::vec4(0, 0, 0, 1);
}

void Controller::toGlobalReference() {
    if(superMode == Mode::World)
        return;
    superMode = Mode::World;
    // calculate eulers for world from current
}
void Controller::toLocalReference() {
    if(superMode == Mode::Point)
        return;
    superMode = Mode::Point;
    // calculate eulers for local from current
}

void Controller::update(const glm::mat4& parentTransform, float dt) {
    if(not hasFocus())
        return;
    console_prefix("Camera");

    if(input.zoom != 0.f)
        zoom();
    fov = fovChange.update(dt);

    rotation = computeTargetRotation(parentTransform, dt);
    origin = computeTargetPosition(parentTransform, dt);
    rotation.update(dt);
    origin.update(dt);

    if(setup.inLocalSpace)
        Camera::orientation = parentTransform * glm::toMat4(rotation.get());
    else
        Camera::orientation = glm::toMat4(rotation.get());
    // apply stabilization
    Camera::orientation[3] = origin.get() + Camera::orientation * offset * offsetScale;
    Camera::recalculate();

    console.flog("position", origin.get());

    ControlInput::resetAfterUse();
}

// todo: add some inertia
void Controller::zoom() {
    if(setup.zoomMode == FOV) {
        fov = input.zoom * 15.f;
    }
    else {
        offsetScale += input.zoom * 2;
    }
}
// todo: description
// todo: include camera position offset in calculations
glm::quat Controller::computeTargetRotation(const glm::mat4& parentTransform, float dt) {
    if(input.worldPointToFocusOn) {
        if(currentMode != Mode::Point) {
            currentMode = Mode::Point;
        }

        // todo: weź pod uwagę offset
        const auto dir = glm::normalize((*input.worldPointToFocusOn - eyePosition()).xyz());
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
    else if(currentMode == Mode::Point) {
        if(superMode == Mode::Local)
            ;
        if(superMode == Mode::World)
            ;
        currentMode = superMode;
    }

    if(worldPointToPivot) {}
    if(worldPointToZoom) {}

    // niestety na razie kąty eulera
    glm::vec2 v(-input.pointer.vertical * cos(-roll) - input.pointer.horizontal * sin(-roll),
                -input.pointer.vertical * sin(-roll) + input.pointer.horizontal * cos(-roll));

    pitch = pitch - (v.x * 12.f * fov) / pi;
    yaw = yaw - (v.y * 12.f * fov) / pi;

    roll = roll + input.pointer.roll;
    // auto out = glm::quat(glm::vec3(*yaw, *pitch, *roll));
    auto out = glm::angleAxis(*yaw, Z3) * glm::angleAxis(*pitch, X3);
    // todo: different order of application?
    if(setup.addRotationToTarget)
        out = extractHorizontalRotation(parentTransform) * out;
    else if(setup.addInclinationToTarget)
        out = extractInclination(parentTransform) * out;
    return out;
}

glm::quat Controller::extractHorizontalRotation(const glm::mat4& parentTransform) const {
    // calculate what? rotation between Y and Y''(Y' projected on horizontal surface(if exists))
    return {};
}
glm::quat Controller::extractInclination(const glm::mat4& parentTransform) const {
    // calculate rotation between Z and Z'
    return {};
}

glm::vec4 Controller::computeTargetPosition(const glm::mat4& parentTransform, float dt) {
    if(not setup.isFreecam)
        return parentTransform[3];

    if(setup.restrictMovementToHorizontalPlane) {
        return origin.get() + (input.velocity.x * right + input.velocity.z * at) * glm::vec4(1, 1, 0, 0)
               + glm::vec4(0, 0, input.velocity.y * .5f, 0) * dt / frameMs;
    }
    else {
        return origin.get() + input.velocity.x * right + input.velocity.y * up + input.velocity.z * at * dt / frameMs;
    }
}

}
