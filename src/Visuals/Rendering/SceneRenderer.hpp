#pragma once

class Camera;
class Context;
class Scene;
class Window;

class SceneRenderer
{
private:
    Window &window;
    Context &context;
public:
    SceneRenderer(Window &window, Context &context) : window(window), context(context){}

    void renderScene(Scene &scene, Camera &camera); // ???
    void renderTerrain(Scene &scene, Camera &camera);
    void renderFoliage(Scene &scene, Camera &camera);
    void renderGrass(Scene &scene, Camera &camera);
};
