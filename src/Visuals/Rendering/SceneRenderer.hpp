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
    void renderSceneStuff(Scene &scene, Camera &camera);
    void renderSkinned(Camera &camera);
    void renderTracks(Camera &camera);
    void renderSkinnedShadows(Scene &scene, Camera &camera);
    void renderTerrain(Scene &scene, Camera &camera);
    void renderFoliage(Scene &scene, Camera &camera);
    void renderGrass(Scene &scene, Camera &camera);

    void renderShadows(Scene &scene, Camera &camera);
    void renderGlossyObjects(Camera &camera);
    void render_SimpleModelPbr(Camera &camera);
public:
    SceneRenderer(Window &window, Context &context) : window(window), context(context){}

    void renderScene(Scene &scene, Camera &camera); // ???
};
