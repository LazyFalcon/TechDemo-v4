#include "core.hpp"
#include "GraphicEngine.hpp"
#include "Context.hpp"
#include "Details.hpp"
#include "Effects.hpp"
#include "GBufferSampler.hpp"
#include "LightRendering.hpp"
#include "RendererUtils.hpp"
#include "SceneRenderer.hpp"
#include "VfxEffects.hpp"
#include "Window.hpp"
#include "ui-renderer.hpp"
#include "visuals-csm.hpp"
#include "visuals-prepared-scene.hpp"

GraphicEngine::GraphicEngine(Window& window)
    : window(window),
      context(std::make_unique<Context>(window)),
      details(std::make_unique<Details>(*context, window)),
      utils(std::make_unique<RendererUtils>(window, *context)),
      effects(std::make_unique<Effects>(window, *context, *utils)),
      gBufferSamplers(std::make_unique<GBufferSamplers>(window, *context)),
      lightRendering(std::make_unique<LightRendering>(window, *context)),
      sceneRenderer(std::make_unique<SceneRenderer>(window, *context)),
      csm(std::make_unique<visuals::CascadedShadowMapping>(window, *context)),
      uiRender(std::make_unique<UIRender>(window, *context)),
      vfxEffects(std::make_unique<VfxEffects>(window, *context)) {
    context->reset();
}

GraphicEngine::~GraphicEngine() {
    console.log("~GraphicEngine");
};
void GraphicEngine::beginFrame() {
    context->beginFrame();
}
void GraphicEngine::endFrame() {
    context->endFrame();
}
void GraphicEngine::takeScreenShot() {
    utils->takeScreenShot();
}
void GraphicEngine::blurAndStoreRenderedScene() {
    utils->blurBuffer();
}
