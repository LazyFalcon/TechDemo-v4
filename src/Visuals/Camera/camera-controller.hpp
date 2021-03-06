#pragma once
#include "camera-control-input.hpp"
#include "camera-data.hpp"
#include "camera-hud-interface.hpp"
#include "camera-utils.hpp"
#include "gradian.hpp"

namespace camera
{
class Controller;
Controller& active();
bool hasActive();

Controller& snapshot();
void makeSnapshot();

class Controller : public Camera, public ControlInput, public CameraHUDInterface
{
private:
    // angles are in camera space
    // in each frame target camera rotation is calculated from them
    Gradian yaw;   // y, around Z axis
    Gradian pitch; // x, around X axis
    Gradian roll;  // z, around Y axis
    // todo: fajniejsze byłoby cos takiego: Limits<AngleLimit(0, pi), Periodic()>

    Utils::ValueFollower<glm::vec4> origin;
    Utils::ValueFollower<glm::quat, Utils::quaternionSlerpFunction> rotation;
    Utils::ValueFollower<Gradian, Utils::defaultMixFunction<float>> fovChange;
    Utils::ValueFollower<Utils::Limits<float>> offsetChange;

    enum class Mode
    {
        World,
        Local,
        Point
    };
    Mode currentMode {Mode::World};
    Mode superMode {Mode::World};

    glm::vec4 calculateEyePositionOffset(const glm::mat4& cameraRelativeMatrix) const;
    void zoom(float dt);
    glm::quat computeTargetRotation(const glm::mat4& parentTransform, float dt);
    glm::vec4 computeTargetPosition(const glm::mat4& parentTransform, float dt);
    glm::quat getRotationBasis(const glm::mat4& parentTransform, float dt);

    glm::quat extractHorizontalRotation(const glm::mat4& parentTransform) const;
    glm::quat extractInclination(const glm::mat4& parentTransform) const;

    void recalculateEulersForWorldReference();
    void recalculateEulersForLocalReference();

public:
    Controller();
    Controller(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize);
    Controller(const glm::mat4& cameraWorldMatrix, glm::vec2 windowSize);
    // Controller(const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize) : Controller(cameraRelativeMatrix, cameraRelativeMatrix, windowSize){}
    virtual ~Controller();
    void printDebug();

    Controller clone();

    void focusOn();
    bool hasFocus() const;

    virtual void update(float dt) {
        update(glm::mat4(1), dt);
    }
    void update(const glm::mat4& parentTransform, float dt);

    void toGlobalReference();
    void toLocalReference();
};

}
