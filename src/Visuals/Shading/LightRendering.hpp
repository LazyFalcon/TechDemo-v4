#pragma once

class Camera;
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

    void renderSun(Sun& sun, Camera &camera);
    void renderLights(Scene &scene, Camera &camera);
    void hemisphericalAmbient(Scene &scene, Camera &camera);
    void renderShinyObjects(Scene &scene, Camera &camera);
    float calculateLuminance();
    void compose(Camera &camera);

    void lightPass(Scene &scene, Camera &camera);

    float lightIntensity {1};
};
