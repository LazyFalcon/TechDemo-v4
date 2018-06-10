#include "core.hpp"
#include "Context.hpp"
#include "GraphicEngine.hpp"
#include "input-dispatcher.hpp"
#include "input.hpp"
#include "ObjectBatchedRender.hpp"
#include "PhysicalWorld.hpp"
#include "Player.hpp"
#include "Playground.hpp"
#include "PlaygroundEvents.hpp"
#include "RendererUtils.hpp"
#include "Settings.hpp"
#include "Texture.hpp"
#include "VehicleBuilder.hpp"
#include "Window.hpp"

Playground::Playground(Imgui& ui, InputDispatcher& inputDispatcher, Window& window):
    m_input(inputDispatcher.createNew("Playground")),
    m_physical(std::make_unique<PhysicalWorld>()),
    m_window(window)
    {
        float defaultCameraVelocity = 2;
        float preciseCameraVelocity = 0.1;

        m_input->action("esc").on([]{
            event<ExitPlayground>();
        });
        m_input->action("f12").on([this]{ CameraController::getActiveCamera().printDebug(); });
        m_input->action("scrollUp").on([this]{ CameraController::getActiveCamera().changeFov(+3*toRad); });
        m_input->action("scrollDown").on([this]{ CameraController::getActiveCamera().changeFov(-3*toRad); });
        m_input->action("+").on([=]{ CameraController::getActiveCamera().offset.z -= 4*defaultCameraVelocity; });
        m_input->action("-").on([=]{ CameraController::getActiveCamera().offset.z += 4*defaultCameraVelocity; });
        m_input->action("shift-+").on([=]{ CameraController::getActiveCamera().offset.z -= 4*preciseCameraVelocity; });
        m_input->action("shift--").on([=]{ CameraController::getActiveCamera().offset.z += 4*preciseCameraVelocity; });
        m_input->action("LMB").on([this]{ m_cameraRotate = true; }).off([this]{ m_cameraRotate = false; });
        m_input->action("MousePosition").on([this](float x, float y){
            m_mousePos = glm::vec2(x,y);
        });
        m_input->action("MouseMove").on([this](float x, float y){
            m_mouseTranslation = glm::vec2(x,y) * m_window.size * 2.f;
            m_mouseTranslationNormalized = glm::vec2(x,y);
        });
        m_input->activate();
    }

Playground::~Playground(){
    m_input->deactivate();
}
void Playground::update(float dt){
    m_player->updateGraphic(dt);
    m_player->graphics.toBeRendered();
}
void Playground::updateWithHighPrecision(float dt){
    m_player->update(dt);

    auto &currentCamera = CameraController::getActiveCamera();
    // if((KeyState::lClicked or KeyState::mouseReset) and not CTRL_MODE)
    if(m_cameraRotate) currentCamera.rotateByMouse(m_mouseTranslationNormalized.x * dt/16.f, m_mouseTranslationNormalized.y * dt/16.f);

    // if(freeCam->hasFocus()){
    //     freeCam->camera.directlyMoveConstZ(velocity, 5.f, dt * 0.001f * 53.f*dt/16.f);
    // }

    for(auto &cam : CameraController::listOf){
        cam->update(dt);
    }
    m_mouseTranslationNormalized = {};
}
Scene&Playground:: getScene(){
    return m_scene;
}
void Playground::renderProcedure(GraphicEngine& renderer){
    renderer.context->beginFrame();
    renderer.context->setupFramebufferForGBufferGeneration();
    // renderer.sceneRenderer->renderScene(scene, CameraController::getActiveCamera());
    // renderer.utils->drawBackground("nebula2");
    renderer.objectBatchedRender->renderObjects(CameraController::getActiveCamera());

    renderer.context->tex.gbuffer.color.genMipmaps();

    // renderer.uiRender->render(m_ui.getToRender());
    renderer.context->endFrame();
}

void Playground::spawnPlayer(const std::string& configName, glm::vec4 position){
    m_player = std::make_shared<Player>(m_input->getDispatcher());

    VehicleBuilder builder(configName, *m_player, *m_physical, m_window.camFactory);
    builder.build();
}
