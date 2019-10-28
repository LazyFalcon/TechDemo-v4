#pragma once

namespace Utils
{
template<typename TT>
TT& defaultClampFunction(TT& value, const std::optional<TT>& min, const std::optional<TT>& max) {
    value = glm::clamp(value, min.value_or(value), max.value_or(value));
    return value;
}

template<typename TT>
TT& no_op(TT& value, const std::optional<TT>& min, const std::optional<TT>& max) {
    return value;
}

template<typename TT>
TT& periodicAngle(TT& value, const std::optional<TT>& min, const std::optional<TT>& max) {
    if(value > pi)
        value -= 2 * pi;
    if(value < -pi)
        value += 2 * pi;

    // auto diff = fmod(value + pi, pi2) - pi; // wrap difference to range -pi;pi
    // return diff < -pi ? diff + pi2 : diff;

    return value;
}

// todo: klaska do obsługi kątów(Angle), taka bliższa matematyce, z operatorami ile się da, okresowa z ograniczeniem

// todo: nie wiem czy to jest do końca takie najszczęśliwsze rozwiązanie
template<typename T, auto WrapFunction = defaultClampFunction<T>>
class Limits
{
private:
    T m_value;
    std::optional<T> m_min {};
    std::optional<T> m_max {};

public:
    Limits(T value) : m_value(value) {}
    Limits(T value, T min, T max) : m_value(value), m_min(min), m_max(max) {}

    void operator=(T v) {
        m_value = v;
        applyLimits();
    }

    operator T() const {
        return m_value;
    }

    T& operator*() {
        return applyLimits();
    }

    const T& get() const {
        return m_value;
    }

    void setBounds(T min, T max) {
        m_min = min;
        m_max = max;
    }

    T& set(const T& val) {
        m_value = val;
        return applyLimits();
    }

    auto& applyLimits() {
        return WrapFunction(m_value, m_min, m_max);
    }
};

template<typename T>
T defaultMixFunction(T value, T target, float miliseconds, float smoothness) {
    return glm::mix(value, target, smoothness * miliseconds / frameMs);
    // return glm::mix(rotationCenter, target.rotationCenter, glm::smoothstep(0.f, 1.f, inertia * dt/frameTime));
    // smoothstep ma sens wtedy gdy przy ustaleniu wartosci, zapiszemy startową wartość, i będziemy robiliinterpolację nie po czasie a po
    // różnicy, znaczy e = t-v0; i v += smootshstep(0, <warość od kiedy zaczynamy wygładzenie>, e);
}

inline glm::quat quaternionSlerpFunction(const glm::quat& value, const glm::quat& target, float miliseconds,
                                         float smoothness) {
    return glm::slerp(value, target, smoothness * miliseconds / frameMs);
}

template<typename T, auto InterpolateFunction = defaultMixFunction<T>>
class ValueFollower
{
private:
    T m_value;
    T m_target;
    float m_smoothness {1};
    float m_inertia {0};

public:
    ValueFollower(T value, float smoothness = 1.f, float inertia = 0.f)
        : m_value(value), m_target(value), m_smoothness(smoothness), m_inertia(inertia) {}

    void operator=(T v) {
        m_target = v;
    }

    operator T() const {
        return m_value;
    }

    T& operator*() {
        return m_value;
    }

    const T& get() const {
        return m_value;
    }

    void set(const T& val) {
        m_target = val;
    }

    void reset(const T& val) {
        m_value = val;
        m_target = val;
    }

    void setParameters(float smoothness, float inertia) {
        m_smoothness = smoothness;
        m_inertia = inertia;
    }

    void modifyValue(T newValue) {
        m_value = newValue;
    }

    const T& update(float dt) {
        m_value = InterpolateFunction(m_value, m_target, dt, m_smoothness);
        return m_value;
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
