#pragma once

class Camera;
class Context;
class Scene;
class Window;

class LightRendering
{
private:
    Window &window;
    Context &context;
public:
    LightRendering(Window &window, Context &context) : window(window), context(context){}

    void renderSun(Scene &scene, Camera &camera);
    void renderLights(Scene &scene, Camera &camera);
    void hemisphericalAmbient(Scene &scene, Camera &camera);
    void renderShinyObjects(Scene &scene, Camera &camera);
    float calculateLuminance();
    void compose(Camera &camera);

    float lightIntensity {1};
};
