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

Controller::Controller(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize)
    : yaw(0),
      pitch(0),
      roll(0),
      // pitch(0, -pi/3, pi/3),
      // roll(0, -pi/2, pi/2),
      // fovLimited(Camera::fov, 30*toRad, 120*toRad),
      origin(parentMatrix[3], 0.1f, 0.5f),
      rotation(glm::quat_cast(parentMatrix * cameraRelativeMatrix), 0.1f, 0.5f) {
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
}

Controller::Controller(const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize)
    : yaw(0),
      pitch(0),
      roll(0),
      // pitch(0, -pi/3, pi/3),
      // roll(0, -pi/2, pi/2),
      // fovLimited(Camera::fov, 30*toRad, 120*toRad),
      origin(cameraRelativeMatrix[3], 0.1f, 0.5f),
      rotation(glm::quat_cast(cameraRelativeMatrix), 0.1f, 0.5f) {
    listOfControllers.push_back(this);
    if(not activeCamera)
        focusOn();

    Camera::aspectRatio = windowSize.x / windowSize.y;
    Camera::nearDistance = 0.10f;
    Camera::farDistance = 1500.f;
    Camera::fov = 85 * toRad;
    Camera::inertia = 1;
    Camera::smoothing = 1;

    console.log("Position:", cameraRelativeMatrix[3]);

    glm::extractEulerAngleXYZ(cameraRelativeMatrix, *pitch, *yaw, *roll);

    offset = glm::vec4(0, 0, -1, 0);
    offsetScale = 1;
    offset = glm::normalize(offset);

    Camera::orientation = glm::toMat4(glm::angleAxis(*yaw, Z3) * glm::angleAxis(*pitch, X3));
    Camera::orientation[3] = origin.get() + Camera::orientation * offset * offsetScale;
    Camera::recalculate();
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
    // console.log("yaw, pitch, roll:", yaw*toDeg, pitch*toDeg, roll*toDeg);
    // console.log("\t", "freecam:", freecam);
    // console.log("\t", "zoomByFov:", zoomByFov);
    // console.log("\t", "keepRightAxisHorizontal:", keepRightAxisHorizontal);
    // console.log("\t", "parentRotationAffectCurrentRotation:", parentRotationAffectCurrentRotation);
    // console.log("\t", "smoothParentRotation:", smoothParentRotation);
    // console.log("\t", "inSteadyFocusOnPoint:", inSteadyFocusOnPoint);
    // console.log("\t", "targetRelativeToParent:", targetRelativeToParent);
    // console.log("\t", "switchToMovementOnWorldAxes:", switchToMovementOnWorldAxes);
    // console.log("\t", "moveHorizontally:", moveHorizontally);
    // console.log("\t", "reqiuresToHavePointerInTheSamePosition:", reqiuresToHavePointerInTheSamePosition);
    // console.log("\t", "reqiuresToHavePointerInTheSamePosition:", reqiuresToHavePointerInTheSamePosition);
    // console.log("\t", "reqiuresToHavePointerInTheSamePosition:", reqiuresToHavePointerInTheSamePosition);
    // console.log("\t", "reqiuresToHavePointerInTheSamePosition:", reqiuresToHavePointerInTheSamePosition);
}

void Controller::update(const glm::mat4& parentTransform, float dt) {
    if(not hasFocus())
        return;

    auto mode = calculateMode();

    recomputeEulersIfModeChanged(mode, parentTransform);
    currentMode = mode;

    console_prefix("Camera");

    if(input.zoom != 0.f)
        zoom();

    rotation = computeTargetRotation(parentTransform, dt);
    origin = computeTargetPosition(parentTransform, dt);
    rotation.update(dt);
    origin.update(dt);

    // if(parentRotationAffectCurrentRotation) applyParentRotationToCurrent(parentTransform, dt);
    if(setup.inLocalSpace)
        Camera::orientation = glm::toMat4(glm::quat_cast(parentTransform) * rotation.get());
    if(setup.inLocalSpaceRotationOnly)
        Camera::orientation = glm::toMat4(glm::quat_cast(parentTransform) * rotation.get());
    if(setup.inLocalSpacePlane)
        Camera::orientation = glm::toMat4(glm::quat_cast(parentTransform) * rotation.get());
    else
        Camera::orientation = glm::toMat4(rotation.get());
    // apply stabilization
    Camera::orientation[3] = origin.get() + Camera::orientation * offset * offsetScale;
    Camera::recalculate();

    console.flog("position", origin.get());

    ControlInput::resetAfterUse();
}

Controller::Mode Controller::calculateMode() {
    if(input.worldPointToFocusOn)
        return Mode::Point;
    // todo: maybeslightly different implementation?
    if(setup.inLocalSpace or setup.inLocalSpaceRotationOnly or setup.inLocalSpacePlane)
        return Mode::Local;
    return Mode::World;
}

// todo: ffinish implementation
void Controller::recomputeEulersIfModeChanged(Controller::Mode newMode, const glm::mat4& parentTransform) {
    if(newMode == currentMode)
        return;

    if(newMode == Mode::Local)
        glm::extractEulerAngleXYZ(Camera::orientation * glm::affineInverse(parentTransform), *pitch, *yaw, *roll);
    if(newMode == Mode::World)
        glm::extractEulerAngleXYZ(Camera::orientation, *pitch, *yaw, *roll);
}

// todo: add some inertia
void Controller::zoom() {
    if(setup.zoomMode == FOV) {
        fov = input.zoom * 15.f;
    }
    else {
        input.zoom * 2;
    }
}
// todo: description
// todo: include camera position offset in calculations
glm::quat Controller::computeTargetRotation(const glm::mat4& parentTransform, float dt) {
    if(input.worldPointToFocusOn) {
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

    // niestety na razie kąty eulera
    glm::vec2 v(input.pointer.vertical * cos(-roll) - input.pointer.horizontal * sin(-roll),
                input.pointer.vertical * sin(-roll) + input.pointer.horizontal * cos(-roll));

    pitch = pitch - (v.x * 12.f * fov) / pi;
    yaw = yaw - (v.y * 12.f * fov) / pi;

    roll = roll + input.pointer.roll;

    auto out = glm::angleAxis(*yaw, Z3) * glm::angleAxis(*pitch, X3);
    return out;
}

// target camera orientation always keep right axis in hoizontal plane
glm::quat Controller::stabilizeHorizontal(glm::quat toStabilize) {
    return toStabilize;
}

glm::vec4 Controller::computeTargetPosition(const glm::mat4& parentTransform, float dt) {
    if(not setup.isFreecam)
        return parentTransform[3];

    if(setup.restrictMovementToHorizontalPlane) {
        return origin.get() + (input.velocity.x * right + input.velocity.z * at) * glm::vec4(1, 1, 0, 0)
               + glm::vec4(0, 0, input.velocity.y * .5f, 0);
    }
    else {
        return origin.get() + input.velocity.x * right + input.velocity.y * up + input.velocity.z * at;
    }
}

}
