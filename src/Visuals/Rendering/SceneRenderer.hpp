#pragma once

class Camera::Camera;
class Context;
class Scene;
class Window;

class SceneRenderer
{
private:
    Window &window;
    Context &context;
    void renderSceneStuff(Scene &scene, Camera::Camera &camera);
    void renderSkinned(Camera::Camera &camera);
    void renderTracks(Camera::Camera &camera);
    void renderSkinnedShadows(Scene &scene, Camera::Camera &camera);
    void renderTerrain(Scene &scene, Camera::Camera &camera);
    void renderFoliage(Scene &scene, Camera::Camera &camera);
    void renderGrass(Scene &scene, Camera::Camera &camera);

    void renderShadows(Scene &scene, Camera::Camera &camera);
    void renderGlossyObjects(Camera::Camera &camera);
    void render_SimpleModelPbr(Camera::Camera &camera);
public:
    SceneRenderer(Window &window, Context &context) : window(window), context(context){}

    void renderScene(Scene &scene, Camera::Camera &camera); // ???
};
