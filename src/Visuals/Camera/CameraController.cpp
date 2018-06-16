#include "core.hpp"
#include "CameraController.hpp"
#include "Constants.hpp"
#include "Logging.hpp"
#include "Utils.hpp"

CameraController* CameraController::activeCamera {nullptr};
std::list<CameraController*> CameraController::listOf;

CameraController::CameraController(glm::vec2 windowSize){
    listOf.push_back(this);
    if(not activeCamera) activeCamera = this;

    // * defaults
    Camera::aspectRatio = windowSize.x/windowSize.y;
    Camera::nearDistance = 0.10f;
    Camera::farDistance = 900.f;
    Camera::fov = 85*toRad;
    Camera::inertia = 1;
    Camera::offset = glm::vec4(0);
    Camera::recalucuateProjectionMatrix();
    Camera::evaluate();
}
CameraController::~CameraController(){
    listOf.remove(this);
    if(activeCamera == this and not listOf.empty()) listOf.front()->focus();
}

void CameraController::focus(){
    activeCamera = this;
}
bool CameraController::hasFocus() const {
    return this == activeCamera;
}

CameraController& CameraController::getActiveCamera(){
    return *activeCamera;
}

// -----------------------------------------------------------------
CopyOnlyPosition::CopyOnlyPosition(glm::vec2 windowSize) : CameraController(windowSize){
    euler = glm::vec3(69*toRad, 14*toRad, 0);
    rotationCenter = glm::vec4(0,0,0,1);
    target.euler = euler;
    target.rotationCenter = rotationCenter;

    constraints.yaw = {{ -pi, pi }, true};
    constraints.pitch = {{ 0 , 160*toRad }};
    constraints.roll = {{ -90*toRad, 90*toRad }};
    constraints.fov = {{ 30*toRad, 120*toRad }};
    constraints.offset = {{{{-5,-5,-5, 0}, {5,5,25, 0}}}};
}

void CopyOnlyPosition::rotateByMouse(float screenX, float screenY, const glm::vec4&){
    // * take into accout camera roll
    glm::vec2 v(screenY*cos(-euler.z) - screenX*sin(-euler.z),
                screenY*sin(-euler.z) + screenX*cos(-euler.z));

    // TODO: is screenX <> euler.y proper math? cleanup this
    target.euler.x -= (v.x * 12.f * fov)/pi;
    target.euler.y -= (v.y * 12.f * fov)/pi;
}

void CopyOnlyPosition::roll(float angle){
    target.euler.z += angle;
}

void CopyOnlyPosition::applyTransform(float dt){
    constraints.yaw(target.euler.y);
    constraints.pitch(target.euler.x);
    constraints.roll(target.euler.z);
    constraints.fov(Camera::fov);
    constraints.offset(Camera::offset);

    euler = glm::mix(euler, target.euler, glm::smoothstep(0.f, 1.f, inertia * dt/16.f));
    // orientation = glm::slerp(orientation, target.basis, basisSmooth*dt/16.f);
    orientation = glm::eulerAngleZ(euler.y) * glm::eulerAngleX(euler.x) * glm::eulerAngleZ(euler.z); // * yaw, pitch, roll
    rotationCenter = glm::mix(rotationCenter, target.rotationCenter, glm::smoothstep(0.f, 1.f, inertia * dt/16.f));

    orientation[3] = rotationCenter + orientation * glm::vec4(offset.x*(0.1f + offset.z/25.f), offset.y*(0.1f + offset.z/25.f), offset.z, 0);
}

void CopyOnlyPosition::update(const glm::mat4& parentTransform, float dt){
    if(not hasFocus()) return;

    target.rotationCenter = parentTransform[3];
    // applyBounds(target.rotationCenter);
    applyTransform(dt);
    Camera::evaluate();
}

void CopyOnlyPosition::printDebug(){
    Camera::printDebug();
    log("euler:", euler.x*toDeg, euler.y*toDeg, euler.z*toDeg);
}

// -----------------------------------------------------------------
CopyTransform::CopyTransform(glm::vec2 windowSize) : CopyOnlyPosition(windowSize){
    euler = glm::vec3(90*toRad, 0, 0);
    constraints.yaw = {{ -pi, pi }, true};
    constraints.pitch = {{ 0 , 160*toRad }};
    constraints.roll = {{ -90*toRad, 90*toRad }};
    constraints.offset = {{{{0,0,0, 0}, {0,0,25, 0}}}};
}

void CopyTransform::applyTransform(const glm::mat4& parentTransform, float dt){
    constraints.yaw(target.euler.y);
    constraints.pitch(target.euler.x);
    constraints.roll(target.euler.z);
    constraints.fov(Camera::fov);
    constraints.offset(Camera::offset);

    // orientation = glm::slerp(orientation, target.basis, basisSmooth*dt/16.f);
    euler = glm::mix(euler, target.euler, glm::smoothstep(0.f, 1.f, inertia * dt/16.f));
    orientation = parentTransform * glm::eulerAngleZ(euler.y) * glm::eulerAngleX(euler.x) * glm::eulerAngleZ(euler.z);
    rotationCenter = glm::mix(rotationCenter, target.rotationCenter, glm::smoothstep(0.f, 1.f, inertia * dt/16.f));

    orientation[3] = rotationCenter + orientation * glm::vec4(offset.x*(0.1f + offset.z/25.f), offset.y*(0.1f + offset.z/25.f), offset.z, 0);
}

void CopyTransform::update(const glm::mat4& parentTransform, float dt){
    if(not hasFocus()) return;

    target.rotationCenter = parentTransform[3];
    // applyBounds(target.rotationCenter);
    applyTransform(parentTransform, dt);
    Camera::evaluate();
}

// -----------------------------------------------------------------
FreeCamController::FreeCamController(glm::vec2 windowSize) : CameraController(windowSize){
    m_cam = glm::angleAxis(0.f, glm::normalize(glm::vec3(1,0,-0.2)));
    m_rotationCenter = glm::vec4(0,0,0,1);
    m_target.rotationCenter = m_rotationCenter;
    m_target.cam = m_cam;
    offset = glm::vec4(0,2,15,0);
    inertia = 0.2;
    m_target.impulse = {};

    constraints.yaw = {{ -pi, pi }, true};
    constraints.pitch = {{ 0 , 160*toRad }};
    constraints.roll = {{ -90*toRad, 90*toRad }};
    constraints.fov = {{ 30*toRad, 120*toRad }};
    constraints.offset = {{{{-5,-5,-5, 0}, {5,5,25, 0}}}};
}
void FreeCamController::update(float dt){
    if(not hasFocus()) return;
    m_target.rotationCenter += m_target.impulse;
    m_target.impulse = {};
    m_rotationCenter = glm::mix(m_rotationCenter, m_target.rotationCenter, glm::smoothstep(0.f, 1.f, inertia * dt/16.f));
    m_cam = glm::slerp(m_cam, m_target.cam, inertia * dt/16.f);

    if(mode == Mode::Around){
        orientation = glm::toMat4(m_cam);
        orientation[3] = m_rotationCenter + orientation * offset;
        // orientation[3] = m_rotationCenter + orientation * glm::vec4(offset.x*(0.1f + offset.z/25.f), offset.y*(0.1f + offset.z/25.f), offset.z, 0);
    }
    else {
        orientation = glm::toMat4(m_cam);
        orientation[3] = m_rotationCenter + orientation * offset;
    }

    // applyTransform(dt);
    Camera::evaluate();
}

void FreeCamController::releaseRotationCenter(){
    if(mode == Mode::Around) return;
    rotationAroundPoint.firstTime = true;
    m_target.cam = m_cam;
    m_rotationCenter = m_target.rotationCenter = position();
    offset = {};
}
void FreeCamController::changeMode(){ // * when '[' is pressed for some time on this camera
    if(mode == Mode::Around){
        mode = Mode::InPlace;

        rotationAroundPoint.storedOffset = offset;
        m_rotationCenter = m_target.rotationCenter = position();
        offset = {};
    }
    else {
        mode = Mode::Around;

        offset = rotationAroundPoint.storedOffset;
        m_rotationCenter = m_target.rotationCenter = position() - orientation * offset;
    }
}

void FreeCamController::zoomToMouse(const glm::vec4& underMousePos){
    if(mode == Mode::Around){
        changeFov(-15*toRad);
    }
    else {
        float d = glm::length(underMousePos - position());
        m_target.rotationCenter += std::max(d*0.1f, 1.f) * (underMousePos - position())/d;
    }
}
void FreeCamController::zoomOutMouse(const glm::vec4& underMousePos){
    if(mode == Mode::Around){
        changeFov(+15*toRad);
    }
    else {
        if(glm::distance(position(), underMousePos) < 1.f) m_target.rotationCenter -= at;
        float d = glm::length(underMousePos - position());
        m_target.rotationCenter -= std::max(d*0.1f, 1.f) * (underMousePos - position())/d;
    }
}
void FreeCamController::rotateByMouse(float screenX, float screenY, const glm::vec4& underMousePos){
    // screenY = abs(screenY) > abs(screenX) ? screenY*4 : 0;
    // screenX = abs(screenX) > abs(screenY) ? screenX*4 : 0;
    float intensity = -5;
    m_target.cam = glm::angleAxis(intensity*screenX, glm::vec3(0,0,1)) * glm::angleAxis(intensity*screenY, glm::rotate(m_target.cam, glm::vec3(1,0,0))) * m_target.cam;
    if(mode == Mode::Around){
    }
    else {
        // * this is best for 6dof camera,
        // m_target.cam = glm::rotate(m_target.cam, intensity*screenY, glm::vec3(1,0,0));
        // m_target.cam = glm::rotate(m_target.cam, intensity*screenX, glm::vec3(0,1,0));
        if(rotationAroundPoint.firstTime and glm::distance(position(), underMousePos) < 100.f){
            rotationAroundPoint.firstTime = false;
            offset = view * (position() - underMousePos);
            m_target.rotationCenter = m_rotationCenter = underMousePos;
        }
    }
}
void FreeCamController::roll(float r){}

// * in cameera loc, so z is at
void FreeCamController::applyImpulse(float x, float y, float z){
    if(mode == Mode::Around){
        // * z and x are in horizontal plane
        // m_target.impulse += glm::vec4(x, y, -z, 0);
        m_target.impulse += (x*right + z*at)*glm::vec4(1,1,0,0) + glm::vec4(0,0,y*.5f,0);
    }
    else {
        m_target.impulse += x*right + y*up + z*at;
    }
}

void FreeCamController::printDebug(){
    Camera::printDebug();
    log("right", right);
    log("up", up);
    log("at", at);
}
