#pragma once
#include "camera-frustum.hpp"

namespace camera
{
template<typename T>
struct constr
{
    std::array<T, 2> minMax;
    bool disabled;
    void operator()(T& t) {
        if(not disabled)
            t = glm::clamp(t, minMax[0], minMax[1]);
    }
};

struct CameraConstraints
{
    constr<float> yaw {{-pi, pi}};
    constr<float> pitch {{-pi * 0.95f, 0}};
    constr<float> roll {{-pi * 0.95f, 0}};
    constr<float> fov {{2.5f * toRad, 150.f * toRad}};
    constr<glm::vec4> offset {{{{-5, -5, -5, 0}, {5, 5, 25, 0}}}};
};
// todo: rename to data, move some logici away
class Camera
{
public:
    // * from camera transform calculate view matrix. call once per frame, before rendering phase
    void recalculate();
    void recalucuateProjectionMatrix();
    void recalucuateFrustum();
    void changeFov(float delta); // ! radians please!
    void printDebug();
    float convertDepthToWorld(float);
    void changeOffset(float x, float y, float z);

    // * controlled params
    glm::mat4 orientation;

    // * control parameters
    float fov;
    float aspectRatio;
    float nearDistance;
    float farDistance;

    float inertia;
    float smoothing;
    glm::vec4 offset; // todo: rename to offsetVector
    float offsetScale;

    // * calculated. each cam has it's own, not sure if useful
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 projectionInverse;
    glm::mat4 PV;
    glm::mat4 invPV;

    glm::vec4 at;    // -Z
    glm::vec4 right; // X
    glm::vec4 up;    // Y

    glm::mat4 getPV() const {
        return PV;
    }
    const glm::vec4& position() const {
        return orientation[3];
    }
    const glm::vec4& eyePosition() const {
        return orientation[3];
    }

    // todo: remove from here!
    Frustum frustum {};
    const Frustum& getFrustum() const {
        return frustum;
    }
};

}
