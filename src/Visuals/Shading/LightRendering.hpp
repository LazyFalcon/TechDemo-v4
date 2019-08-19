#pragma once

namespace camera{ class Camera; }
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

    void renderSun(Sun& sun, camera::Camera &camera);
    void renderPointLights(camera::Camera &camera);
    void renderConeLights(camera::Camera &camera);
    void hemisphericalAmbient(Scene &scene, camera::Camera &camera);
    void renderShinyObjects(Scene &scene, camera::Camera &camera);
    float calculateLuminance();
    void compose(camera::Camera &camera);

    void lightPass(Scene &scene, camera::Camera &camera);

    float lightIntensity {1};
};
