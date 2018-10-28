#include "core.hpp"
#include "RenderDataCollector.hpp"
#include "ModelLoader.hpp"
#include "Constants.hpp"
#include "LightSource.hpp"
#include "Camera.hpp"
#include "Window.hpp"

RenderDataCollector::tupleOfContainers RenderDataCollector::collection;
CommandArray RenderDataCollector::enviro;
CommandArray RenderDataCollector::foliage;
Uniforms RenderDataCollector::uniforms;
std::vector<LightSource*> RenderDataCollector::lights[100];


void RenderDataCollector::collectCamera(Camera& camera){
    uniforms.uFovTan = (float)tan(camera.fov*0.5f);
    uniforms.uNear = camera.nearDistance;
    uniforms.uFar = camera.farDistance;
    uniforms.uView = camera.view;
    uniforms.uInvPV = camera.invPV;
    uniforms.uEyePosition = camera.position();
    uniforms.exposture = 1.f;
    uniforms.gamma = 2.2f;
}
void RenderDataCollector::collectWindow(Window& window){
    uniforms.uWindowSize = window.size;
    uniforms.uPixelSize = window.pixelSize;
}
void RenderDataCollector::collectTime(float lastFrame, u64 sinceStart){
    uniforms.lastFrameTime = lastFrame;
    uniforms.sinceStartTime = sinceStart;
}
