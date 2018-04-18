#include "Context.hpp"
#include "GraphicEngine.hpp"
#include "LightRendering.hpp"
#include "Effects.hpp"
#include "RendererUtils.hpp"
#include "SceneRenderer.hpp"
#include "ShadowCaster.hpp"
#include "ObjectBatchedRender.hpp"
#include "UIDrawer.hpp"
#include "GBufferSampler.hpp"
#include "VfxEffects.hpp"
#include "Window.hpp"

GraphicEngine::GraphicEngine(Window &window, UI::Updater &uiUpdater) :
    window(window),
    context(std::make_unique<Context>(window)),
    utils(std::make_unique<RendererUtils>(window, *context)),
    lightRendering(std::make_unique<LightRendering>(window, *context)),
    effects(std::make_unique<Effects>(window, *context, *utils)),
    vfxEffects(std::make_unique<VfxEffects>(window, *context)),
    sceneRenderer(std::make_unique<SceneRenderer>(window, *context)),
    shadowCaster(std::make_unique<ShadowCaster>(window, *context)),
    uiDrawer(std::make_unique<UIDrawer>(window, *context, uiUpdater)),
    gBufferSamplers(std::make_unique<GBufferSamplers>(window, *context)),
    objectBatchedRender(std::make_unique<ObjectBatchedRender>(window, *context))
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
