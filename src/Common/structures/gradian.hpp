#pragma once
#include "Constants.hpp"

inline float rad(float a) {
    return a * pi / 180.f;
}

inline float grad(float a) {
    return a / (pi * 2.f);
}

inline float dec(float a) {
    return a * 180.f / pi;
}

inline float grad2dec(float a) {
    return a * 2.f * 180.f;
}

// todo: użyj może typedefów do lepszego opisu typu zmiennych?
using radian = float;
using gradian = float;
using degree = float;
using force = float;
using velocity = float;

// better representation of grad, more stable in calculations(multiplicity of pi), in range -0.5f to 0.5f
class Gradian
{
    float m_virtualZero; // position of virtual zero, so limits on grad will never cross point of wrap\
        (this between -0.5 and 0.5), lies in the middle of limits
    float m_range;       // maximal delta from virtual zero
    float m_delta;       // delta from virtual zero

public:
    Gradian(radian radians) : m_virtualZero(0), m_range(0.5f), m_delta(wrap(grad(radians))) {}
    Gradian(radian radians, radian lower, radian upper) {
        lower = wrap(grad(lower));
        upper = wrap(grad(upper));

        if(upper > lower) {
            m_range = (upper - lower) / 2.f;
            m_virtualZero = (upper + lower) / 2.f;
        }
        else {
            m_range = (1.f - lower + upper) / 2.f;
            m_virtualZero = lower + m_range;
        }
        m_delta = wrap(grad(radians)) - m_virtualZero;
    }

    operator float() const {
        return m_delta;
    }

    void operator+=(float in) {
        m_delta = limit(wrap(m_delta + in));
    }
    void operator-=(float in) {
        m_delta = limit(wrap(m_delta - in));
    }
    void operator*=(float in) {
        m_delta = limit(wrap(m_delta * in));
    }
    void operator/=(float in) {
        m_delta = limit(wrap(m_delta / in));
    }

    radian operator*() const { // ? what should be correct behaviour here?
        return asRad();
    }

    radian asRad() const {
        return wrap(m_virtualZero + m_delta) * 2.f * pi;
    }
    degree asDec() const {
        return dec(wrap(m_virtualZero + m_delta) * 2.f * pi);
    }

    void fromRads(radian rads) {
        m_delta = limit(wrap(grad(rads)));
    }
    // todo: pozostałe operatory matematyczne, albo chociaż +=, -=, *= i /=
    // todo: i operatory porównań: >,< <=, >=, ==
    void operator=(gradian grads) {
        m_delta = limit(wrap(grads));
    }

    gradian limit(gradian gradian) const {
        return glm::clamp(gradian, -m_range, m_range);
    }

    gradian wrap(gradian gradian) const {
        float i;
        gradian = modff(gradian, &i);

        if(gradian > 0.5f)
            gradian -= 1.f;
        else if(gradian <= -0.5f)
            gradian += 1.f;

        return gradian;
    }
};
