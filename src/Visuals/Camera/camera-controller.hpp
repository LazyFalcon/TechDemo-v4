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

    // potem można to wydzielić do jakichś podstanów
    typedef glm::vec4 (Controller::*positionControlFunc)(const glm::mat4&, ControlInput&, float);
    typedef glm::quat (Controller::*rotationControlFunc)(const glm::mat4&, ControlInput&);
    typedef void (Controller::*initStateFunc)(ControlInput&);

    template<typename T>
    struct LocalState
    {
        std::string name;
        initStateFunc init;
        T func;
    };
    using positionControlState = LocalState<positionControlFunc>;
    using rotationControlState = LocalState<rotationControlFunc>;

    static std::map<std::string, positionControlState, std::less<>> positionControlStates;
    static std::map<std::string, rotationControlState, std::less<>> rotationControlStates;

    positionControlState positionControl;
    rotationControlState rotationControl;

    void initState(ControlInput&){};
    void setState(std::string_view name){
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

    void updateMovement(const glm::mat4& parentTransform, float dt){
        origin = std::invoke(positionControl.func, this, parentTransform, *this, dt);
        rotation = std::invoke(rotationControl.func, this, parentTransform, *this);
        rotation.update(dt);
        origin.update(dt);
        // update filters
        // update exposture and camera aperture
        // view depth changes with exposture :D
    }

    void recalculateCamera(){
        Camera::orientation = glm::toMat4(rotation.get());
        Camera::orientation[3] = origin.get() + Camera::orientation * offset;

        Camera::recalculate();
    }

public:
    Controller(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize);
    virtual ~Controller();

    void focusOn();
    bool hasFocus() const;

    void setBehavior(std::string_view type){
        auto idx = type.find('-');
        setState(type.substr(0, idx));
        setState(type.substr(idx+1, type.size()-idx-1));
    }

    // void rotateInViewPlane(float horizontal, float vertical){
    //     glm::vec2 v(vertical*cos(-roll.get()) - horizontal*sin(-roll.get()),
    //                 vertical*sin(-roll.get()) + horizontal*cos(-roll.get()));

    //     pitch = pitch - (v.x * 12.f * fov)/pi;
    //     yaw = yaw - (v.y * 12.f * fov)/pi;
    // }

    // void roll(float angle){
    //     roll = roll + angle;
    // }

    virtual void update(float dt) {
        update(glm::mat4(1), dt);
    }
    void update(const glm::mat4& parentTransform, float dt){
        if(not hasFocus()) return;
        updateMovement(parentTransform, dt);
        recalculateCamera();
    }

    // void modView(float change){
    //     // if(mode == ZOOM_BY_FOV) modFov(change);
    //     // else if(mode == ZOOM_BY_DISTANCE) modDistance(change);
    // }

    // void modFov(float scaleChange){
    //     // make scale change non linear - when closer steps are smaller, but in a way that is reversible
    // }

    // void modDistance(float scaleChange){
    //     // make scale change non linear - when closer steps are smaller, but in a way that is reversible
    // }

    void printDebug();
};

}
