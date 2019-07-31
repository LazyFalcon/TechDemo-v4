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
    virtual void rotateByMouse(float, float, const glm::vec4&){}
    virtual void roll(float){}
    virtual void changeMode(){}
    virtual void zoomToMouse(const glm::vec4&){}
    virtual void zoomOutMouse(const glm::vec4&){}
    virtual void releaseRotationCenter(){}
    virtual void applyImpulse(float, float, float){}
    virtual void update(float dt){}
    virtual void update(const glm::mat4& parentTransform, float dt){}

    void focus();
    bool hasFocus() const;

    virtual void printDebug(){}
    static CameraController& getActiveCamera();
    static std::list<CameraController*> listOf;
};

// * 2 DoF in worldspace, intended to follow objects
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
    void calculateEyePositionOffset(const glm::mat4& initialPosition);
public:
    CopyOnlyPosition(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize);
    void applyTransform(float);
    void rotateByMouse(float, float, const glm::vec4&);
    void roll(float);
    void update(const glm::mat4& parentTransform, float dt) override;
    void printDebug() override;
};

// * 2 DoF in object space
// class CopyTransformHorizontal : public CopyOnlyPosition
class CopyTransform : public CopyOnlyPosition
{
private:
public:
    CopyTransform(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize);
    void applyTransform(const glm::mat4&, float) ;
    void update(const glm::mat4& parentTransform, float dt) override;
};

// * Freecam, rotates around point, with offset, zooming is in direction of offset
class FreeCamController : public CameraController
{
private:
    struct {
        glm::vec4 rotationCenter;
        glm::quat cam;
        glm::vec4 impulse;
    } m_target;

    CameraConstraints constraints;
    glm::vec4 m_rotationCenter;
    glm::quat m_cam;

    struct {
        bool firstTime;
        glm::vec4 storedRotCeter;
        glm::vec4 storedOffset;
    } rotationAroundPoint;

    enum class Mode {Around, InPlace} mode {Mode::Around};

public:
    FreeCamController(const glm::mat4& cameraMatrix, glm::vec2 windowSize);
    void update(float dt) override;
    void zoomToMouse(const glm::vec4&);
    void zoomOutMouse(const glm::vec4&);
    void rotateByMouse(float, float, const glm::vec4&);
    void releaseRotationCenter();
    void applyImpulse(float, float, float);
    void roll(float);
    void copyFrom(CameraController&); // * when '[' is pressed for some time on different camera
    void changeMode();
    void printDebug() override;
};
