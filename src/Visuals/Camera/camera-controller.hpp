#pragma once
#include "camera.hpp"
#include "camera-utils.hpp"
#include "camera-hud-interface.hpp"
#include "camera-control-input.hpp"

namespace Camera
{

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

    // potem można to wydzielić do jakichś podstanów
    typedef glm::vec4 (Controller::*positionControlFunc)(const glm::mat4&, ControlInput&, float);
    typedef glm::quat (Controller::*rotationControlFunc)(const glm::mat4&, ControlInput&);
    typedef void (Controller::*initStateFunc)(ControlInput&);

    template<typename T>
    struct LocalState
    {
        std::string name;
        initStateFunc init;
        T control;
    };
    using positionControlState = LocalState<positionControlFunc>;
    using rotationControlState = LocalState<rotationControlFunc>;

    static std::map<std::string, positionControlState> positionControlStates;
    static std::map<std::string, rotationControlState> rotationControlStates;

    positionControlState positionControl;
    rotationControlState rotationControl;

    void initState(ControlInput&){};
    void setstate(const std::string& name){
        if(auto it = positionControlStates.find(name); it != positionControlStates.end() and positionControl.name != name){
            positionControl = it->second;
            std::invoke(positionControl.init, this, *this);
        }
        else if(auto it = rotationControlStates.find(name); it != rotationControlStates.end() and rotationControl.name != name){
            rotationControl = it->second;
            std::invoke(rotationControl.init, this, *this);
        }
    }

    glm::vec4 freecamPosition(const glm::mat4&, ControlInput&, float);
    glm::vec4 pinnedPosition(const glm::mat4&, ControlInput&, float);

    glm::quat global_euler(const glm::mat4&, ControlInput&);
    glm::quat global_euler_copyUp(const glm::mat4&, ControlInput&);
    glm::quat local_euler(const glm::mat4&, ControlInput&);
    glm::quat local_euler_stablized(const glm::mat4&, ControlInput&);
    glm::quat local_focused(const glm::mat4&, ControlInput&);
    glm::quat local_focused_stabilized(const glm::mat4&, ControlInput&);

    glm::vec4 calculateEyePositionOffset(const glm::mat4& cameraRelativeMatrix) const {
        // matrix describes camera relative position in space of module, so now we need to inverse camera matrix to get distance of module origin on each camera axis
        auto inv = glm::affineInverse(cameraRelativeMatrix);
        return inv[3] - glm::vec4(0,0,0,1);
    }

public:
    Controller(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize):
        yaw(0),
        pitch(0, -pi/3, pi/3),
        roll(0, -pi/2, pi/2),
        fovLimited(fov, 30*toRad, 120*toRad),
        origin(parentMatrix[3], 0.1f, 0.5f),
        rotation(glm::angleAxis(0.f, Z3), 0.1f, 0.5f)
    {
        glm::extractEulerAngleXYZ(cameraRelativeMatrix, *pitch, *yaw, *roll);
        offset = calculateEyePositionOffset(cameraRelativeMatrix);
        recalculateCamera();
    }

    void updateMovement(const glm::mat4& parentTransform, float dt){
        rotation = std::invoke(rotationControl, this, parentTransform, *this, dt);
        origin = std::invoke(positionControl, this, parentTransform, *this);
        rotation.update(dt);
        origin.update(dt);
        // update filters
        // update exposture and camera aperture
        // view depth changes with exposture :D
    }

    void recalculateCamera(){
        orientation = glm::toMat4(orientation);
        orientation[3] = origin + orientation * offset;

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
        updateMovement(parentTransform, dt);
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

private:
    std::variant<> movementPolicy;
    std::unique_ptr<ControlPolicy> orientationPolicy;
};

}
