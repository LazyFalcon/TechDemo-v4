#include "core.hpp"
#include "CameraController.hpp"
#include "Context.hpp"
#include "Effects.hpp"
#include "GBufferSampler.hpp"
#include "GraphicEngine.hpp"
#include "input-dispatcher.hpp"
#include "input.hpp"
#include "LightRendering.hpp"
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
    m_window(window),
    m_mouseSampler(std::make_unique<GBufferSampler>())
    {
        float defaultCameraVelocity = 2;
        float preciseCameraVelocity = 0.1;

        m_defaultCamera = std::make_shared<FreeCamController>(m_window.size);
        m_defaultCamera->focus();

        m_mouseSampler->samplePosition = glm::vec2(0,0);
        float m_freecamSpeed = 0.5f;

        m_input->action("esc").on([]{
            event<ExitPlayground>();
        });
        m_input->action("f12").on([this]{
            CameraController::getActiveCamera().printDebug();
            log("m_mouseWorldPos", m_mouseWorldPos);
            });
        m_input->action("+").on([this]{ CameraController::getActiveCamera().offset.z -= 1.5; });
        m_input->action("-").on([this]{ CameraController::getActiveCamera().offset.z += 1.5; });
        m_input->action("scrollUp").on([=]{
                if(m_useFreecam) m_defaultCamera->zoomToMouse(m_mouseSampler->position);
                else CameraController::getActiveCamera().changeFov(+15*toRad);
            });
        m_input->action("scrollDown").on([=]{
                if(m_useFreecam) m_defaultCamera->zoomOutMouse(m_mouseSampler->position);
                else CameraController::getActiveCamera().changeFov(-15*toRad);
            });
        m_input->action("RMB").on([this]{
                // TODO: move freecam, in a way that mouse world position is preserved
            });
        m_input->action("LMB").on([this]{
                m_cameraRotate = true;
                m_mouseWorldPos = m_mouseSampler->position;
            }).off([this]{
                m_cameraRotate = false;
                if(m_useFreecam) m_defaultCamera->releaseRotationCenter();
            });
        m_input->action("Q").on([this]{ CameraController::getActiveCamera().roll(-15*toRad); });
        m_input->action("E").on([this]{ CameraController::getActiveCamera().roll(+15*toRad); });

        m_input->action("W").hold([this, m_freecamSpeed]{ m_defaultCamera->applyImpulse(0,0,m_freecamSpeed); });
        m_input->action("S").hold([this, m_freecamSpeed]{ m_defaultCamera->applyImpulse(0,0,-m_freecamSpeed); });
        m_input->action("A").hold([this, m_freecamSpeed]{ m_defaultCamera->applyImpulse(-m_freecamSpeed,0,0); });
        m_input->action("D").hold([this, m_freecamSpeed]{ m_defaultCamera->applyImpulse(m_freecamSpeed,0,0); });
        m_input->action("Z").hold([this, m_freecamSpeed]{ m_defaultCamera->applyImpulse(0,m_freecamSpeed,0); });
        m_input->action("X").hold([this, m_freecamSpeed]{ m_defaultCamera->applyImpulse(0,-m_freecamSpeed,0); });
        m_input->action("\\").hold([this]{
            if(m_input->currentKey.onHoldTime > 700 and m_useFreecam){
                m_input->currentKey.release = true;
                m_defaultCamera->changeMode();
                log("state changed");
            }
            else if(m_input->currentKey.onHoldTime > 700 and not m_useFreecam){
                // TODO: create freecam from current camera
            }
         });
        m_input->action("P").hold([this]{
            if(m_input->currentKey.onHoldTime > 700){
                if(m_useFreecam){
                    m_player->focusOn();
                    m_useFreecam = false;
                }
                else {
                    m_useFreecam = true;
                    m_player->focusOff();
                    m_defaultCamera->focus();
                }
            }
         });

        m_input->action("MousePosition").on([this](float x, float y){
            m_mousePos = glm::vec2(x,y);
            m_mouseSampler->samplePosition = m_mousePos;
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
    if(m_useFreecam and m_defaultCamera->hasFocus()){ // * I hope player doesn't have control over it's cameras
        m_defaultCamera->focus();
    }

    m_player->update(dt);

    auto &currentCamera = CameraController::getActiveCamera();
    // if((KeyState::lClicked or KeyState::mouseReset) and not CTRL_MODE)
    if(m_cameraRotate) currentCamera.rotateByMouse(m_mouseTranslationNormalized.x * dt/16.f, m_mouseTranslationNormalized.y * dt/16.f, m_mouseWorldPos);

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


    renderer.gBufferSamplers->sampleGBuffer(CameraController::getActiveCamera());

    renderer.context->setupFramebufferForLighting();

    renderer.effects->SSAO(CameraController::getActiveCamera());

    renderer.lightRendering->compose(CameraController::getActiveCamera());

    // renderer.context->setupFramebufferForLDRProcessing();
    // renderer.effects->toneMapping(1.f/*CameraController::getActiveCamera().exposure*/);
    renderer.effects->FXAA();

    renderer.context->tex.gbuffer.color.genMipmaps();

    // renderer.uiRender->render(m_ui.getToRender());
    renderer.context->endFrame();
}

void Playground::spawnPlayer(const std::string& configName, glm::vec4 position){
    m_player = std::make_shared<Player>(m_input->getDispatcher());

    VehicleBuilder builder(configName, *m_player, *m_physical, m_window.camFactory);
    builder.build();
}
