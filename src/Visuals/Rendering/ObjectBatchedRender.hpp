#pragma once

class Camera;
class Context;
class GraphicDataCollector;
class ObjectBatchedRender;
class SkinnedMesh;
class ArmoredVehicleTracks;
class Window;
class Scene;

class ObjectBatchedRender
{
private:
    Window &window;
    Context &context;

    void renderSkinned(Camera &camera);
    void renderTracks(Camera &camera);
    void renderSkinnedShadows(Scene &scene, Camera &camera);

    void render_SimpleModelPbr(Camera &camera);
public:
    ObjectBatchedRender(Window &window, Context &context) : window(window), context(context)
        {}

    void renderObjects(Scene &scene, Camera &camera);
    void renderShadows(Scene &scene, Camera &camera);
    void renderGlossyObjects(Camera &camera);

};