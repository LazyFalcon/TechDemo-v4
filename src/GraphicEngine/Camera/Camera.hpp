#pragma once
#include "Frustum.hpp"
#include "Filters.hpp"
// TODO: refactor names
class Camera;

struct CameraConstraints
{
    void update(Camera &camera, float dt);

    glm::vec2 horizontal { -pi, pi };
    glm::vec2 vertical { -pi * 0.95f , 0  };
    glm::vec2 fov { 2.5f * toRad, 150.f * toRad };
    glm::vec2 distance { 0.f, 35.f };
    bool use { true };
};

struct CameraTargetEvaluator
{
    void update(Camera &camera, float dt);

    float vertical { 0.2f };
    float horizontal { 0.2f };
    float fov { 0.2f };
    float distance { 0.2f };

    float basisSmooth { 0.09f };
    float positionSmooth { 0.09f };

    glm::quat basis { 0,0,0,1 };
    glm::quat parentBasis { 0,0,0,1 };
    glm::vec4 position { 0,0,0,1 };
    glm::vec4 rotationCenter { 0,0,0,1 };
    bool use { true };
};

class Camera
{
protected:
    ~Camera() = default;
public:
    // * from camera transform calculate view matrix. call once per frame, before rendering phase
    void evaluate();
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
    glm::vec4 offset;

    // * calculated. each cam has it's own, not sure if useful
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 projectionInverse;
    glm::mat4 PV;
    glm::mat4 invPV;

    glm::vec4 at; // -Z
    glm::vec4 right; // X
    glm::vec4 up; // Y

    glm::mat4 getPV() const {
        return PV;
    }
    const glm::vec4& position() const {
        return orientation[3];
    }

    Frustum frustum {};
    const Frustum& getFrustum() const {
        return frustum;
    }
};
