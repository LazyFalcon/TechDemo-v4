#pragma once

#include "Camera.hpp"
#include <list>

class CommonHUDData;
class Window;

class CameraController : public Camera
{
private:
    static CameraController *activeCamera;
public:
    CameraController();
    CameraController(const std::string &smode);
    virtual ~CameraController();
    // CameraController(const Camera &cam) : camera(cam){}
    void updateBaseTransform(const glm::mat4 &tr){
        baseTransform = tr;
    }
    void move();
    void rotate();
    virtual void update(float dt) = 0;
    void focus();
    bool hasFocus() const;

    // Camera camera;
    glm::vec4 offsetPosition;
    glm::vec2 focusPoint;
    glm::mat4 baseTransform;
    static Camera& getActiveCamera();
    static std::list<CameraController*> listOf;
};

class FollowOnlyPosition : public CameraController
{
     void update(float dt) override {}
};

class FollowPlane : public CameraController
{
     void update(float dt) override {}
};

class FollowFull : public CameraController
{
     void update(float dt) override {}
};


class FreeCamController : public CameraController
{
public:
    FreeCamController();
    void update(float dt) override;
};

class FollowingCamController : public CameraController
{
public:
    FollowingCamController();
    void update(float dt) override;
};

class PinnedCamController : public CameraController
{
public:
    PinnedCamController();
    void update(float dt) override;
};

class TopdownCamController : public CameraController
{
public:
    TopdownCamController();
    void update(float dt) override;
};
