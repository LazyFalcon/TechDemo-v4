#pragma once
namespace camera
{
class Camera;
}
class Context;
class Scene;
class Window;

class SceneRenderer
{
private:
    Window& window;
    Context& context;
    void renderSceneStuff(Scene& scene, camera::Camera& camera);
    void renderSkinned(camera::Camera& camera);
    void renderTracks(camera::Camera& camera);
    void renderSkinnedShadows(Scene& scene, camera::Camera& camera);
    void renderTerrain(Scene& scene, camera::Camera& camera);
    void renderFoliage(Scene& scene, camera::Camera& camera);
    void renderGrass(Scene& scene, camera::Camera& camera);

    void renderShadows(Scene& scene, camera::Camera& camera);
    void renderGlossyObjects(camera::Camera& camera);
    void render_SimpleModelPbr(camera::Camera& camera);

public:
    SceneRenderer(Window& window, Context& context) : window(window), context(context) {}

    void renderScene(Scene& scene, camera::Camera& camera); // ???
};
