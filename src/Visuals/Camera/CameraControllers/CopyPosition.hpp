#pragma once
#include "CameraController.hpp"

namespace Utils
{
template<typename TT>
TT& defaultClampFunction(TT& value, const TT& min, const TT& max){
    if(min < max) value = glm::clamp(value, min, max);
    return value;
}

template<typename TT>
TT& no_op(TT& value, const TT& min, const TT& max){
    return value;
}

template<typename T, auto WrapFunction = defaultClampFunction<T>>
class Limits
{
private:
    std::remove_reference_t<T> m_min {};
    std::remove_reference_t<T> m_max {};
    T value;
public:
    Limits(T value) : value(value){}
    template<typename T2, typename T3>
    Limits(T value, T2 min, T3 max) : m_min(min), m_max(max), value(value){}

    template<typename T3>
    void operator=(T3 v){
        value = v;
        update();
    }

    operator T() const {
        return value;
    }

    template<typename T2, typename T3>
    void setBounds(T2 min, T3 max){
        m_min = min;
        m_max = max;
    }

    T& operator * (){
        return WrapFunction(value, m_min, m_max);
    }

    T& set(const T& val){
        value = val;
        return update();
    }

    auto update(){
        return WrapFunction(value, m_min, m_max);
    }
};

template<typename T>
T defaultMixFunction(const T& value, const T&  target, float miliseconds, float smoothness){
    const float frameTime = 1000.f/60.f;
    return glm::mix(value, target, smoothness * miliseconds/frameTime);
    // return glm::mix(rotationCenter, target.rotationCenter, glm::smoothstep(0.f, 1.f, inertia * dt/frameTime));
    // smoothstep ma sens wtedy gdy przy ustaleniu wartosci, zapiszemy startową wartość, i będziemy robiliinterpolację nie po czasie a po
    // różnicy, znaczy e = t-v0; i v += smootshstep(0, <warość od kiedy zaczynamy wygładzenie>, e);
}

glm::quat quaternionSlerpFunction(const glm::quat&  value, const glm::quat&  target, float miliseconds, float smoothness){
    const float frameTime = 1000.f/60.f;
    return glm::slerp(value, target, smoothness * miliseconds/frameTime);
}

template<typename T, typename HightType = T, auto InterpolateFunction = defaultMixFunction<T>>
class ValueFollower
{
private:
    float m_smoothness {1};
    float m_inertia {0};
    std::remove_reference_t<T> m_target {};
    HightType value;
public:
    ValueFollower(HightType value, float smoothness, float inertia) : m_smoothness(smoothness), m_inertia(inertia), m_target(value), value(value){}

    template<typename T3>
    void operator=(T3 v){
        m_target = v;
    }
    template<typename T3>
    void set(const T3& v){
        value = v;
        m_target = value;
    }

    operator T() const {
        return value;
    }

    template<typename T2, typename T3>
    void setBounds(T2 smoothness, T3 inertia){
        m_smoothness = smoothness;
        m_inertia = inertia;
    }

    T& set(const T& val){
        m_target = val;
    }

    void update(float dt){
        value =  InterpolateFunction(value, m_target, dt, m_smoothness);
    }
};

/*
 Usage:
    using namespace Utils;
    float value(10);
    Limits<float&> l(value, 0, 15.f);
    Limits<float> l4(56, 0, 15.f);
    Limits<float&, no_op<float>> l2(value, 0, 15.f);
    Limits<float> l3(value, 0, 15.f);
    ValueFollower<float, Limits<float, float&>> f(l, 0.1f, 1);
    ValueFollower<float, Limits<float&>&> f2(l, 0.06f, 1);
    ValueFollower<glm::quat, glm::quat, quaternionSlerpFunction> q(glm::angleAxis(3.14f, glm::vec3(0,0,1)), 0.06f, 1.f);

 */

}

class CopyOnlyPosition2 : public CameraController
{
protected:
    using namespace Utils;
    Utils::Limits<float, no_op<float>> yaw;
    Utils::Limits<float> pitch;
    Utils::Limits<float> roll;
    Utils::Limits<float&> fovLimited;

    Utils::ValueFollower<glm::vec4> origin;

    glm::vec4 rotationCenter;
    glm::vec3 euler;
    CameraConstraints constraints;

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
    CopyOnlyPosition2(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize)
        : CameraController(windowSize),
        yaw(0),
        pitch(0, -pi/3, pi/3),
        roll(0, -pi/2, pi/2),
        fovLimited(fov, 30*toRad, 120*toRad)
    {
        glm::extractEulerAngleXYZ(cameraRelativeMatrix, *pitch, *yaw, *roll);

        rotationCenter = parentMatrix[3];
        target.rotationCenter = rotationCenter;

        // constraints.fov = {{ 30*toRad, 120*toRad }};
        // constraints.offset = {{{{-5,-5,-5, 0}, {5,5,25, 0}}}};

        fromOriginToEye = calculateEyePositionOffset(parentMatrix[3], cameraRelativeMatrix);

        applyTransform(0);
        Camera::evaluate();
    }
    void applyTransform(float dt){
        // constraints.yaw(target.euler.y);
        // constraints.pitch(target.euler.x);
        // constraints.roll(target.euler.z);
        // constraints.fov(Camera::fov);
        // constraints.offset(Camera::offset);


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
