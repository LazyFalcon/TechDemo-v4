#pragma once
#include "camera-data.hpp"
#include "camera-utils.hpp"
#include "camera-hud-interface.hpp"
#include "camera-control-input.hpp"

namespace camera
{

class Controller;
Controller& active();

class Controller: public Camera,
                  public ControlInput,
                  public CameraHUDInterface
{
private:
    Utils::Limits<float, Utils::periodicAngle<float>> yaw; // y, around Z axis
    Utils::Limits<float> pitch; // x, around X axis
    Utils::Limits<float> roll; // z, around Y axis
    Utils::Limits<float&> fovLimited;

    Utils::ValueFollower<glm::vec4> origin;
    Utils::ValueFollower<glm::quat, glm::quat, Utils::quaternionSlerpFunction> rotation;

    glm::vec4 calculateEyePositionOffset(const glm::mat4& cameraRelativeMatrix) const {
        // matrix describes camera relative position in space of module, so now we need to inverse camera matrix to get distance of module origin on each camera axis
        auto inv = glm::affineInverse(cameraRelativeMatrix);
        return inv[3] - glm::vec4(0,0,0,1);
    }

    void handleInput(const glm::mat4& parentTransform, float dt);
    void zoom();

    glm::quat computeTargetRotation(const glm::mat4& parentTransform, float dt);
    glm::vec4 computeTargetPosition(const glm::mat4& parentTransform, float dt);
    glm::quat getRotationBasis(const glm::mat4& parentTransform, float dt)
    void alterTargetRotation();

public:
    Controller(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize);
    Controller(const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize) : Controller(glm::mat4(1), cameraRelativeMatrix, type, windowSize){}
    virtual ~Controller();

    void focusOn();
    bool hasFocus() const;

    virtual void update(float dt) {
        update(glm::mat4(1), dt);
    }
    void update(const glm::mat4& parentTransform, float dt);

    void printDebug();
};

}
