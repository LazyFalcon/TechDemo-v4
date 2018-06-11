﻿#pragma once
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
    virtual void rotateByMouse(float, float){}
    virtual void roll(float){}
    virtual void update(float dt){}
    virtual void update(const glm::mat4& parentTransform, float dt){}

    void focus();
    bool hasFocus() const;

    virtual void printDebug(){}
    static CameraController& getActiveCamera();
    static std::list<CameraController*> listOf;
};

// * 2 DoF in worldspace
class CopyOnlyPosition : public CameraController
{
protected:
    struct {
        glm::vec4 rotationCenter;
        glm::vec3 euler;
    } target;

    glm::vec4 rotationCenter;
    glm::vec3 euler;
    CameraConstraints constraints;
public:
    CopyOnlyPosition(glm::vec2 windowSize);
    void applyTransform(float);
    void rotateByMouse(float, float);
    void roll(float);
    void update(const glm::mat4& parentTransform, float dt) override;
};

// * 2 DoF in object space
class CopyTransform : public CopyOnlyPosition
{
private:
public:
    CopyTransform(glm::vec2 windowSize);
    void applyTransform(const glm::mat4&, float) ;
    void update(const glm::mat4& parentTransform, float dt) override;
    void printDebug() override;
};


class FreeCamController : public CameraController
{
public:
    FreeCamController(glm::vec2 windowSize);
    void update(float dt) override;
};
