#include "core.hpp"
#include "Context.hpp"
#include "Effects.hpp"
#include "GBufferSampler.hpp"
#include "GraphicEngine.hpp"
#include "LightRendering.hpp"
#include "RenderDataCollector.hpp"
#include "RendererUtils.hpp"
#include "SceneRenderer.hpp"
#include "ShadowCaster.hpp"
#include "ui-renderer.hpp"
#include "VfxEffects.hpp"
#include "Window.hpp"

GraphicEngine::GraphicEngine(Window &window) :
    window(window),
    context(std::make_unique<Context>(window)),
    utils(std::make_unique<RendererUtils>(window, *context)),
    effects(std::make_unique<Effects>(window, *context, *utils)),
    gBufferSamplers(std::make_unique<GBufferSamplers>(window, *context)),
    lightRendering(std::make_unique<LightRendering>(window, *context)),
    sceneRenderer(std::make_unique<SceneRenderer>(window, *context)),
    shadowCaster(std::make_unique<ShadowCaster>(window, *context)),
    uiRender(std::make_unique<UIRender>(window, *context)),
    vfxEffects(std::make_unique<VfxEffects>(window, *context))
    {
        context->reset();
    }

GraphicEngine::~GraphicEngine(){
    log("~GraphicEngine");
};
void GraphicEngine::beginFrame(){context->beginFrame();}
void GraphicEngine::endFrame(){context->endFrame();}
void GraphicEngine::takeScreenShot(){
    utils->takeScreenShot();
}
void GraphicEngine::blurAndStoreRenderedScene(){
    utils->blurBuffer();
}
