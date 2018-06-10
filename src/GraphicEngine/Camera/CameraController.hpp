#pragma once
#include "Camera.hpp"
#include <list>

class CommonHUDData;

class CameraController : public Camera
{
private:
    static CameraController *activeCamera;
public:
    CameraController(glm::vec2 windowSize);
    virtual ~CameraController();
    // CameraController(const Camera &cam) : camera(cam){}
    void updateBaseTransform(const glm::mat4 &tr){
        baseTransform = tr;
    }
    void move();
    virtual void rotateByMouse(float, float){}
    virtual void roll(float){}
    virtual void update(float dt){}
    virtual void update(const glm::mat4& parentTransform, float dt){}
    void focus();
    bool hasFocus() const;

    // Camera camera;
    glm::vec4 offsetPosition;
    glm::vec2 focusPoint;
    glm::mat4 baseTransform;
    static CameraController& getActiveCamera();
    static std::list<CameraController*> listOf;
};

// * 2 DoF in worldspace
class CopyOnlyPosition : public CameraController
{
private:
    struct {
        glm::vec4 rotationCenter;
        glm::vec3 euler;
    } target;

    glm::vec4 rotationCenter;
    glm::vec3 euler;

    void applyTransform(float);
public:
    CopyOnlyPosition(glm::vec2 windowSize);
    void rotateByMouse(float, float);
    void roll(float);
    void update(const glm::mat4& parentTransform, float dt) override;
};

class CopyPlane : public CameraController
{
public:
    CopyPlane(glm::vec2 windowSize) : CameraController(windowSize){}
    void update(const glm::mat4& parentTransform, float dt) override;
};

// * 2 DoF in object space
class CopyTransorm : public CameraController
{
public:
    CopyTransorm(glm::vec2 windowSize) : CameraController(windowSize){}
    void update(const glm::mat4& parentTransform, float dt) override;
};


class FreeCamController : public CameraController
{
public:
    FreeCamController(glm::vec2 windowSize);
    void update(float dt) override;
};
