#pragma once

class LightRendering;
class Effects;
class RendererUtils;
class SceneRenderer;
class ShadowCaster;
class ObjectBatchedRender;
class UIRender;
class Window;
class Context;
class GBufferSamplers;
class VfxEffects;

class GraphicEngine
{
private:
    Window &window;
public:
    GraphicEngine(Window &window);
    ~GraphicEngine();
    void reset();
    void beginFrame();
    void endFrame();
    void takeScreenShot();
    void blurAndStoreRenderedScene();

    std::unique_ptr<Context> context;
    std::unique_ptr<RendererUtils> utils;
    std::unique_ptr<UIRender> uiRender;
    std::unique_ptr<LightRendering> lightRendering;
    std::unique_ptr<SceneRenderer> sceneRenderer;
    std::unique_ptr<ShadowCaster> shadowCaster;
    std::unique_ptr<ObjectBatchedRender> objectBatchedRender;
    std::unique_ptr<Effects> effects;
    std::unique_ptr<VfxEffects> vfxEffects;
    std::unique_ptr<GBufferSamplers> gBufferSamplers;
};
