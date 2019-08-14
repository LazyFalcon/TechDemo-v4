#pragma once
#include "CameraController.hpp"
#include "CamcontrolUtils.hpp"

class FreeCam2 : public CameraController
{
protected:
    using namespace Utils;
    Utils::Limits<float, periodicAngle<float>> yaw; // y, around Z axis
    Utils::Limits<float> pitch; // x, around X axis
    Utils::Limits<float> roll; // z, around Y axis
    Utils::Limits<float&> fovLimited;

    Utils::ValueFollower<glm::vec4> origin;
    Utils::ValueFollower<glm::quat, glm::quat, quaternionSlerpFunction> rotation;

    glm::vec4 calculateEyePositionOffset(const glm::vec4& cameraRelativeMatrix) const {
        // matrix describes camera relative position in space of module, so now we need to inverse camera matrix to get distance of module origin on each camera axis
        auto inv = glm::affineInverse(cameraRelativeMatrix);
        return inv[3]-glm::vec4(0,0,0,1);
    }

public:
    FreeCam2(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize)
        : CameraController(windowSize),
        yaw(0),
        pitch(0, -pi/3, pi/3),
        roll(0, -pi/2, pi/2),
        fovLimited(fov, 30*toRad, 120*toRad),
        origin(parentMatrix[3], 0.1f, 0.5f)
        rotation(glm::quat(0), 0.1f, 0.5f)
    {
        glm::extractEulerAngleXYZ(cameraRelativeMatrix, *pitch, *yaw, *roll);
        offset = calculateEyePositionOffset(parentMatrix[3], cameraRelativeMatrix);

        recalculateCamera();
    }

    void updateMovement(float dt){
        rotation = glm::angleAxis(yaw, Z3) * glm::angleAxis(target.euler.x, X3);
        rotation.update(dt);
        origin.update(dt);
        // update filters
        // update exposture and camera aperture
        // view depth changes with exposture :D
    }

    void recalculateCamera(){
        target.transform = glm::angleAxis(target.euler.y, Z3) * glm::angleAxis(target.euler.x, X3);

        orientation = glm::toMat4(transform);
        orientation[3] = rotationCenter + orientation * offset;

        Camera::recalculate();
    }

    void rotateInViewPlane(float horizontal, float vertical){
        glm::vec2 v(vertical*cos(-roll) - horizontal*sin(-roll),
                    vertical*sin(-roll) + horizontal*cos(-roll));

        pitch -= (v.x * 12.f * fov)/pi;
        yaw -= (v.y * 12.f * fov)/pi;
    }

    void roll(float angle){
        target.euler.z += angle;
    }

    void update(const glm::mat4& parentTransform, float dt){
        if(not hasFocus()) return;
        updateMovement(dt);
        recalculateCamera();
    }

    void modView(float change){
        if(mode == ZOOM_BY_FOV) modFov(change);
        else if(mode == ZOOM_BY_DISTANCE) modDistance(change);
    }

    void modFov(float scaleChange){
        // make scale change non linear - when closer steps are smaller, but in a way that is reversible
    }

    void modDistance(float scaleChange){
        // make scale change non linear - when closer steps are smaller, but in a way that is reversible
    }

    void printDebug(){
        Camera::printDebug();
        console.log("yaw, pitch, roll:", yaw*toDeg, pitch*toDeg, roll*toDeg);
    }
};


