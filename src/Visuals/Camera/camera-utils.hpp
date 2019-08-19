#pragma once

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

    T get() const {
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
    return glm::mix(value, target, smoothness * miliseconds/frameMs);
    // return glm::mix(rotationCenter, target.rotationCenter, glm::smoothstep(0.f, 1.f, inertia * dt/frameTime));
    // smoothstep ma sens wtedy gdy przy ustaleniu wartosci, zapiszemy startową wartość, i będziemy robiliinterpolację nie po czasie a po
    // różnicy, znaczy e = t-v0; i v += smootshstep(0, <warość od kiedy zaczynamy wygładzenie>, e);
}

glm::quat quaternionSlerpFunction(const glm::quat&  value, const glm::quat&  target, float miliseconds, float smoothness){
    return glm::slerp(value, target, smoothness * miliseconds/frameMs);
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

    T get(){
        return value;
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
