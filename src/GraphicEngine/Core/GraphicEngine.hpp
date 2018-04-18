#pragma once

class LightRendering;
class Effects;
class RendererUtils;
class SceneRenderer;
class ShadowCaster;
class ObjectBatchedRender;
class UIDrawer;
class Window;
class Context;
class GBufferSamplers;
class VfxEffects;
namespace UI {class IMGUI; class Updater;}

class GraphicEngine
{
private:
    Window &window;
public:
    GraphicEngine(Window &window, UI::Updater &uiUpdater);
    ~GraphicEngine();
    void reset();
    void beginFrame();
    void endFrame();
    void takeScreenShot();
    void blurAndStoreRenderedScene();

    std::unique_ptr<Context> context;
    std::unique_ptr<RendererUtils> utils;
    std::unique_ptr<UIDrawer> uiDrawer;
    std::unique_ptr<LightRendering> lightRendering;
    std::unique_ptr<SceneRenderer> sceneRenderer;
    std::unique_ptr<ShadowCaster> shadowCaster;
    std::unique_ptr<ObjectBatchedRender> objectBatchedRender;
    std::unique_ptr<Effects> effects;
    std::unique_ptr<VfxEffects> vfxEffects;
    std::unique_ptr<GBufferSamplers> gBufferSamplers;
};
