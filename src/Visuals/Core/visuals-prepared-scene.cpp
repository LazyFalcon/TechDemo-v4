#include "core.hpp"
#include "visuals-prepared-scene.hpp"
#include "Constants.hpp"
#include "GraphicEngine.hpp"
#include "ModelLoader.hpp"
#include "Window.hpp"
#include "camera-data.hpp"

namespace visuals
{
PreparedScene preparedScene;

void PreparedScene::collectCamera(camera::Camera& camera) {
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

void PreparedScene::collectWindow(Window& window) {
    uniforms.uWindowSize = window.size;
    uniforms.uPixelSize = window.pixelSize;
}

void PreparedScene::collectTime(float lastFrame, u64 sinceStart) {
    uniforms.lastFrameTime = lastFrame;
    uniforms.sinceStartTime = sinceStart;
}

void PreparedScene::clear() {
    nonPlayableInsideFrustum.clear();
    nonPlayableOutsideFrustum.clear();
}
}