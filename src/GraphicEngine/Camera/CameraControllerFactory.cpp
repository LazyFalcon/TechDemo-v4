#include <string>
#include "CameraControllerFactory.hpp"
#include "CameraController.hpp"
#include "Window.hpp"

void CameraControllerFactory::initCamera(CameraController &controller){
    controller.camera.aspect = window.size.x/window.size.y;
    controller.camera.nearDistance = 0.10f;
    controller.camera.farDistance = 1500.f;
    controller.camera.fov = 95; // TODO: extract this from app.settingsgameSettings().video.camera.fov * toRad;
    controller.camera.calcProjection();
    controller.camera.positionBounds = glm::vec4(-1000, 1000, -1000, 1000);
    controller.camera.calc(0.1);
    controller.focusPoint = window.center;
}
