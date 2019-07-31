#pragma once
#include "CameraController.hpp"

template<typename T>
struct ValueFollower()
{
    T value;
    T target;
    // float& smoothness;
    float smoothness;
    // float& inertia;
    float inertia;

    void set(T val){
        target = val;
    }
    const T& get(){
        return value;
    }
    const T& update(float dt){

        value = glm::mix(value, target, glm::smoothstep(0.f, 1.f, smoothness * dt/0.016.f));

        return value;
    }

}

class CopyOnlyPosition : public CameraController
{
protected:
    struct {
        glm::vec4 rotationCenter;
        glm::vec3 euler;
    } target;
    glm::vec4 rotationCenter;
    glm::vec3 euler;
    CameraConstraints constraints;

    ValueFollower<glm::vec4> origin;
    glm::vec4 fromOriginToEye;
    glm::vec4 eyePosition;
    glm::quat orientation;
    bool areConstraintsInLocalSpaceOfFollowedObject {false};
    glm::vec4 calculateEyePositionOffset(const glm::vec4& cameraRelativeMatrix) const {
        // matrix describes camera relative position in space of module, so now we need to inverse camera matrix to get distance of module origin on each camera axis
        auto inv = glm::affineInverse(cameraRelativeMatrix);
        return inv[3]-glm::vec4(0,0,0,1);
    }

public:
    CopyOnlyPosition(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize)
    : CameraController(windowSize)
    {
        euler = glm::vec3(69*toRad, 14*toRad, 0);
        glm::extractEulerAngleXYZ(initialPosition, euler.x, euler.y, euler.z);
        rotationCenter = initialPosition[3];
        target.euler = euler;
        target.rotationCenter = rotationCenter;

        constraints.yaw = {{ -pi, pi }, true};
        constraints.pitch = {{ 0 , 160*toRad }};
        constraints.roll = {{ -90*toRad, 90*toRad }};
        constraints.fov = {{ 30*toRad, 120*toRad }};
        constraints.offset = {{{{-5,-5,-5, 0}, {5,5,25, 0}}}};

        fromOriginToEye = calculateEyePositionOffset(parentMatrix[3], cameraRelativeMatrix);

        applyTransform(0);
        Camera::evaluate();
    }
    void applyTransform(float dt){
        constraints.yaw(target.euler.y);
        constraints.pitch(target.euler.x);
        constraints.roll(target.euler.z);
        constraints.fov(Camera::fov);
        constraints.offset(Camera::offset);


        euler = glm::mix(euler, target.euler, glm::smoothstep(0.f, 1.f, inertia * dt/16.f));
        // orientation = glm::slerp(orientation, target.basis, basisSmooth*dt/16.f);
        orientation = glm::eulerAngleZ(euler.y) * glm::eulerAngleX(euler.x) * glm::eulerAngleZ(euler.z); // * yaw, pitch, roll
        // rotationCenter = glm::mix(rotationCenter, target.rotationCenter, glm::smoothstep(0.f, 1.f, inertia * dt/16.f));
        origin.update(dt);

        orientation[3] = origin.get() + orientation * (offset * offsetScale);
    }
    void rotateByMouse(float screenX, float screenY, const glm::vec4&){
        // * take into accout camera roll
        glm::vec2 v(screenY*cos(-euler.z) - screenX*sin(-euler.z),
                    screenY*sin(-euler.z) + screenX*cos(-euler.z));

        // TODO: is screenX <> euler.y proper math? cleanup this
        target.euler.x -= (v.x * 12.f * fov)/pi;
        target.euler.y -= (v.y * 12.f * fov)/pi;
    }
    void roll(float angle){
        target.euler.z += angle;
    }
    void update(const glm::mat4& parentTransform, float dt){
        if(not hasFocus()) return;

        // target.rotationCenter = parentTransform[3];
        origin.set(parentTransform[3]);
        // applyBounds(target.rotationCenter);
        applyTransform(dt);
        Camera::evaluate();
    }
    void printDebug(){
        Camera::printDebug();
        console.log("euler:", euler.x*toDeg, euler.y*toDeg, euler.z*toDeg);
    }

    void zoomByFov(float scaleChange){
        // make scale change non linear - when closer steps are smaller, but in a way that is reversible
    }
    void zoomByDistance(float scaleChange){
        // make scale change non linear - when closer steps are smaller, but in a way that is reversible
    }
};
