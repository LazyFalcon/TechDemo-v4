#include "core.hpp"
#include "CameraControllerFactory.hpp"
#include "CameraController.hpp"
#include "Window.hpp"

CameraControllerFactory::CameraControllerFactory(Window &window) : windowSize(window.size){}

// void CameraControllerFactory::dummyInitCamera(Camera &camera){
    // camera.aspect = window.size.x/window.size.y;
    // camera.nearDistance = 0.10f;
    // camera.farDistance = 900.f;
    // camera.fov = 85; // TODO: extract this from app.settingsgameSettings().video.camera.fov * toRad;
    // camera.positionBounds = glm::vec4(-1000, 1000, -1000, 1000);
    // camera.evaluate();
    // controller.focusPoint = window.center;
// }
