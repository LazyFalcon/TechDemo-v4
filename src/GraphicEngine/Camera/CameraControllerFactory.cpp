#include <string>
#include "CameraControllerFactory.hpp"
#include "CameraController.hpp"
#include "Window.hpp"

void CameraControllerFactory::dummyInitCamera(Camera &camera){
    camera.aspect = window.size.x/window.size.y;
    camera.nearDistance = 0.10f;
    camera.farDistance = 900.f;
    camera.fov = 85; // TODO: extract this from app.settingsgameSettings().video.camera.fov * toRad;
    camera.calcProjection();
    camera.positionBounds = glm::vec4(-1000, 1000, -1000, 1000);
    camera.evaluate(0.1);
    // controller.focusPoint = window.center;
}
