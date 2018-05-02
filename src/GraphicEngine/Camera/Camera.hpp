#pragma once
/**
 *  Implementation of camera with gimbal lock, useful for following objects
 *  To remove gimbal locktransform camera basis
 *  ----
 *  origin: center of rotation, camera position is rotated around origin
 *  position: position of camera, local
 *  basis: camera local basis
 *
 *
 */
// take a look at: https://github.com/ocornut/imgui/issues/123
#include "common.hpp"
#include "Constants.hpp"
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
    glm::vec4 position { 0,0,0,1 };
    glm::vec4 origin { 0,0,0,1 };
    bool use { true };
};

struct Exposure
{
    float target {20};
    float acomodation {0.9f};
    void update(float &exposure, float dt);
};

class Camera
{
public:
    Camera(){

        auto rot = glm::rotate(euler.y, X3) * glm::rotate(euler.x, Z3);
        target.basis = glm::angleAxis(euler.y, X3) * glm::angleAxis(euler.x, Z3);
        target.position = origin + glm::vec4(0,0,distanceToOrigin,0)*(glm::rotate(euler.y, X3) * glm::rotate(euler.x, Z3));

        basis = target.basis;
        position = target.position;
        view = glm::toMat4(basis) * glm::translate(-position.xyz());

        calcProjection();
    }
    enum State { FREE, PIN_POSITION, PIN_TOTAL };
    void update(glm::vec4 position);
    void update(glm::mat4 orientation);
    void update(glm::vec4 normal, glm::vec4 perpendicular);
    void update(float dt);
    void calc(float dt);
    void calcProjection();
    void applyConstraints();
    void updateOscillator(float dt);
    void targetMove(float dt);
    float convertDepthToWorld(float);
    void showDebug() const;

    // appllies force to camera position
    void applyImpulse(glm::vec4 impulse);

    void rotate(glm::vec2 v);
    void move(glm::vec4 v);
    // set origin position, no oscillations
    void setOrigin(glm::vec4 o);
    // targeting
    void setPosition(glm::vec4 o);
    // set origin position, causes ocillations
    void moveOriginTo(glm::vec4 o);

    void updateSmootPosition(glm::vec4 newOrigin);

    glm::mat4 getPV() const {
        return PV;
    }
    void zoomFov(float dy);
    void zoomDistance(float dy);

    void calcFrustum();
    const Frustum& getFrustum() const {
        return frustum;
    }
    void cutZ(glm::vec4 position);

    // debug methods
    glm::vec4 directlyMove(glm::vec4 cameraSpaceVelocities, float dt);
    glm::vec4 directlyMoveConstZ(glm::vec4 cameraSpaceVelocities, float z,  float dt);
    void setBaseOrientation(const glm::mat4 &mat){
        baseOrientation = mat;
    }
    glm::vec4 getX(){
        return X * basis;
    }
    glm::vec4 getY(){
        return Y * basis;
    }
    glm::vec4 getZ(){
        return Z * basis;
    }

    State state = PIN_POSITION;

    // OGL Utils
    glm::mat4 projection;
    glm::mat4 projectionInv;
    glm::mat4 view;
    glm::mat4 viewInv;
    glm::mat4 invPV;
    glm::mat4 PV;

    glm::vec4 at; // -Z
    glm::vec4 right; // X
    glm::vec4 up; // Y

    float nearDistance;
    float farDistance;
    float fov;
    float aspect {1};
    // collected
    float farHeight;
    float farWidth;

    float referenceHeihgt {0};
    bool cropCameraHeight { false };

    CameraConstraints constraints;
    CameraTargetEvaluator target;

    Frustum frustum {};

    float distanceToOrigin { 10.f };
    glm::vec4 postOffset {0,0,0,0};
    glm::vec4 offset {0,0,0,0};

    glm::vec4 origin {0,0,0,1};
    glm::vec4 position {0,0,0,1};
    glm::quat basis = qIdentity;
    glm::quat specQuat = qIdentity;
    glm::mat4 baseOrientation = identity; //
    glm::vec3 euler {0, -1.5, 0};

    glm::vec4 positionBounds; // -x, x, -y, y

    Exposure exposureEvaluator {10, 0.9f};
    float exposure {20};
    void calculateExposure(float luminance);

    Filters filters;
};