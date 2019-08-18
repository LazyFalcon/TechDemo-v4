#pragma once

class Camera::Camera;
class Context;
class Scene;
class Sun;
class Window;

class LightRendering
{
private:
    Window &window;
    Context &context;
public:
    LightRendering(Window &window, Context &context) : window(window), context(context){}

    void renderSun(Sun& sun, Camera::Camera &camera);
    void renderPointLights(Camera::Camera &camera);
    void renderConeLights(Camera::Camera &camera);
    void hemisphericalAmbient(Scene &scene, Camera::Camera &camera);
    void renderShinyObjects(Scene &scene, Camera::Camera &camera);
    float calculateLuminance();
    void compose(Camera::Camera &camera);

    void lightPass(Scene &scene, Camera::Camera &camera);

    float lightIntensity {1};
};
