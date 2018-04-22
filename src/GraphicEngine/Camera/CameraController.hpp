#pragma once

#include "Camera.hpp"
#include <list>

class CommonHUDData;
class Window;

class CameraController
{
private:
    static CameraController *activeCamera;
public:
    CameraController();
    CameraController(const std::string &smode);
    ~CameraController();
    CameraController(const Camera &cam) : camera(cam){}
    void updateBaseTransform(const glm::mat4 &tr){
        baseTransform = tr;
    }
    void move();
    void rotate();
    virtual void update(float dt) = 0;
    void focus();
    bool hasFocus() const;

    Camera camera;
    glm::vec4 offsetPosition;
    glm::vec2 focusPoint;
    glm::mat4 baseTransform;
    static Camera& getActiveCamera();
    static std::list<CameraController*> listOf;
};

class FreeCamController : public CameraController
{
public:
    FreeCamController();
    FreeCamController(const Camera &cam) : FreeCamController(){
        camera = cam;
    }
    virtual void update(float dt);
};

class FollowingCamController : public CameraController
{
public:
    FollowingCamController();
    FollowingCamController(const Camera &cam) : FollowingCamController(){
        camera = cam;
    }
    virtual void update(float dt);
};

class PinnedCamController : public CameraController
{
public:
    PinnedCamController();
    PinnedCamController(const Camera &cam) : PinnedCamController(){
        camera = cam;
    }
    virtual void update(float dt);
};

class TopdownCamController : public CameraController
{
public:
    TopdownCamController();
    TopdownCamController(const Camera &cam) : TopdownCamController(){
        camera = cam;
    }
    virtual void update(float dt);
};
