#include "Context.hpp"
#include "GraphicEngine.hpp"
#include "input-dispatcher.hpp"
#include "input.hpp"
#include "Player.hpp"
#include "Playground.hpp"
#include "PlaygroundEvents.hpp"
#include "RendererUtils.hpp"
#include "Settings.hpp"
#include "Texture.hpp"
#include "VehicleBuilder.hpp"

Playground::Playground(Imgui& ui, InputDispatcher& inputDispatcher): m_input(inputDispatcher.createNew("Playground")){
    m_input->action("esc").on([]{
        event<ExitPlayground>();
    });
    m_input->activate();
}
Playground::~Playground(){
    m_input->deactivate();
}
void Playground::update(float dt){

}
void Playground::updateWithHighPrecision(float dt){}
Scene&Playground:: getScene(){
    return m_scene;
}
void Playground::renderProcedure(GraphicEngine& renderer){
    renderer.context->beginFrame();
    renderer.context->setupFramebufferForGBufferGeneration();
    // renderer.sceneRenderer->renderScene(scene, CameraController::getActiveCamera());
    renderer.utils->drawBackground("nebula2");

    renderer.context->tex.gbuffer.color.genMipmaps();

    // renderer.uiRender->render(m_ui.getToRender());
    renderer.context->endFrame();
}

void Playground::spawnPlayer(const std::string& configName, glm::vec4 position){
    VehicleBuilder builder(configName);
    m_player = std::make_shared<Player>(m_input->getDispatcher());

    builder.build(*m_player);
}
