#include "core.hpp"
#include "RenderDataCollector.hpp"
#include "Constants.hpp"
#include "GraphicEngine.hpp"
#include "LightSource.hpp"
#include "ModelLoader.hpp"
#include "Window.hpp"
#include "camera-data.hpp"

Collected collected;

GraphicEngine* RenderDataCollector::enginePtr;
Details& RenderDataCollector::details() {
    return *(enginePtr->details);
}

void RenderDataCollector::collectCamera(camera::Camera& camera) {
    cameraOfThisFrame = camera;
    uniforms.uFovTan = (float)tan(camera.fov * 0.5f);
    uniforms.uNear = camera.nearDistance;
    uniforms.uFar = camera.farDistance;
    uniforms.uView = camera.view;
    uniforms.uPV = camera.PV;
    uniforms.uInvPV = camera.invPV;
    uniforms.uEyePosition = camera.position();
    uniforms.exposture = 1.f;
    uniforms.gamma = 2.2f;
}
void RenderDataCollector::collectWindow(Window& window) {
    uniforms.uWindowSize = window.size;
    uniforms.uPixelSize = window.pixelSize;
}
void RenderDataCollector::collectTime(float lastFrame, u64 sinceStart) {
    uniforms.lastFrameTime = lastFrame;
    uniforms.sinceStartTime = sinceStart;
}
