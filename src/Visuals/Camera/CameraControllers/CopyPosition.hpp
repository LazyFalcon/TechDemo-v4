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

template<typename TT>
TT& periodicAngle(TT& value, const TT&, const TT&){
    if(value > pi) value -= 2*pi;
    if(value < -pi) value += 2*pi;

    // auto diff = fmod(value + pi, pi2) - pi; // wrap difference to range -pi;pi
    // return diff < -pi ? diff + pi2 : diff;

    return value;
}
// todo: nie wiem czy to jest do końca takie najszczęśliwsze rozwiązanie
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

    T& reset(const T& val){
        value = val;
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


/*
Opcje:
    |> dwa sposoby przybliżania: fov i dystans
    |> reagowanie na zachowanie sledzonego obiektu: żadne, kopiowanie poziomego obrotu

    |> dodatkowo chciałbym zrobić by kamerą obracać za pomocą punktu w który ma patrzeć,
       Zadaniem kontrolera byłoby patrzenie się w ten punkt.
       Obracając kamerę myszą, obracamy wektor którym celujemy, wektor trafia gdzieś w przestrzeń, w ten punk ma się kamera patrzeć

    todo: na starcie konwertuj offset na wektor i wartość skali
    ? a jak zrobić by zoomować fow i dystans niejednorodnie, ale powtarzalnie? trzeb jakąś fcjęnapisać, w głowie nie wymyślę
    ? a jakby rozbić to na komponenty? np Controller(ViewByPoint, CopyOrientation, FollowPosition)?
*/
class CopyOnlyPosition2 : public CameraController
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
    CopyOnlyPosition2(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize)
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
