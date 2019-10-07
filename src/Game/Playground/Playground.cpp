#include "core.hpp"
#include "AI.hpp"
#include "camera-controller.hpp"
#include "Context.hpp"
#include "Details.hpp"
#include "Effects.hpp"
#include "FrameTime.hpp"
#include "GBufferSampler.hpp"
#include "GraphicEngine.hpp"
#include "input-dispatcher.hpp"
#include "input.hpp"
#include "input-user-pointer.hpp"
#include "LightRendering.hpp"
#include "PhysicalWorld.hpp"
#include "Player.hpp"
#include "Playground.hpp"
#include "PlaygroundEvents.hpp"
#include "RenderDataCollector.hpp"
#include "RendererUtils.hpp"
#include "Scene.hpp"
#include "SceneRenderer.hpp"
#include "Settings.hpp"
#include "Texture.hpp"
#include "VehicleAssembler.hpp"
#include "Window.hpp"

Playground::Playground(Imgui& ui, InputDispatcher& inputDispatcher, Window& window, InputUserPointer& inputUserPointer):
    m_input(inputDispatcher.createNew("Playground")),
    m_physics(std::make_unique<PhysicalWorld>()),
    m_window(window),
    m_inputUserPointer(inputUserPointer),
    m_scene(std::make_unique<Scene>(*m_physics, m_window.camFactory)),
    m_mouseSampler(std::make_unique<GBufferSampler>())
    {
        float defaultCameraVelocity = 2;
        float preciseCameraVelocity = 0.1;

        m_mouseSampler->samplePosition = glm::vec2(0,0);
        float m_freecamSpeed = 0.5f;
        // todo: ustawić stan wskaźnika na odpowiedni

        m_input->action("esc").on([]{
            event<ExitPlayground>();
        });
        m_input->action("f12").on([this]{
            camera::active().printDebug();
            console.log("m_mouseWorldPos", m_mouseWorldPos);
            });
        m_input->action("+").on([this]{ camera::active().zoomDirection -= 1.5; });
        m_input->action("-").on([this]{ camera::active().zoomDirection += 1.5; });
        m_input->action("scrollUp").on([=]{
                if(m_freeView) m_scene->freeCams.getController().worldPointToZoom = m_mouseSampler->position;
                // if(m_freeView) m_scene->freeCams.getController().zoomToMouse(m_mouseSampler->position);
                else camera::active().zoomDirection = 1;
            });
        m_input->action("scrollDown").on([=]{
                if(m_freeView) m_scene->freeCams.getController().worldPointToZoom = m_mouseSampler->position;
                else camera::active().zoomDirection = -1;
            });
        m_input->action("RMB").on([this]{
                // TODO: move freecam, in a way that mouse world position is preserved
                auto& cam = camera::active();
                if(not cam.isPointerMovingFree) cam.worldPointToFocusOn = m_mouseSampler->position;
                else cam.rotateAroundThisPoint = m_mouseSampler->position;
                cam.reqiuresToHavePointerInTheSamePosition = true;
            }).off([this]{
                auto& cam = camera::active();
                if(not cam.isPointerMovingFree) cam.worldPointToFocusOn.reset();
                else cam.rotateAroundThisPoint.reset();
                cam.reqiuresToHavePointerInTheSamePosition = false;

            });
        m_input->action("LMB").on([this]{
                m_cameraRotate = true;
                if(m_freeView) camera::active().rotateAroundThisPoint = m_mouseSampler->position;
            }).off([this]{
                m_cameraRotate = false;
                if(m_freeView) camera::active().rotateAroundThisPoint.reset();
            });
        m_input->action("Q").on([this]{ camera::active().pointerMovement.roll = -15*toRad; });
        m_input->action("E").on([this]{ camera::active().pointerMovement.roll = +15*toRad; });
        m_input->action("ctrl").on([this]{ m_inputUserPointer.showSystemCursor(); })
                               .off([this]{ m_inputUserPointer.hideSystemCursor(); });

        m_input->action("W").hold([this, m_freecamSpeed]{ camera::active().directionOfMovement.z = m_freecamSpeed; });
        m_input->action("S").hold([this, m_freecamSpeed]{ camera::active().directionOfMovement.z = -m_freecamSpeed; });
        m_input->action("A").hold([this, m_freecamSpeed]{ camera::active().directionOfMovement.x = -m_freecamSpeed; });
        m_input->action("D").hold([this, m_freecamSpeed]{ camera::active().directionOfMovement.x = m_freecamSpeed; });
        m_input->action("Z").hold([this, m_freecamSpeed]{ camera::active().directionOfMovement.y = m_freecamSpeed; });
        m_input->action("X").hold([this, m_freecamSpeed]{ camera::active().directionOfMovement.y = -m_freecamSpeed; });
        m_input->action("f5").name("global direction").hold([this]{ camera::active().targetRelativeToParent = false; });
        m_input->action("f6").name("local direction").hold([this]{ camera::active().targetRelativeToParent = true; });
        m_input->action("f7").name("enable stabilization").hold([this]{ camera::active().keepRightAxisHorizontal = !camera::active().keepRightAxisHorizontal;
                                            console.log("stabilization:", camera::active().keepRightAxisHorizontal); });
        m_input->action("f8").name("copy parent rotation").hold([this]{ camera::active().parentRotationAffectCurrentRotation = !camera::active().parentRotationAffectCurrentRotation;
                                            console.log("parentRotationAffectCurrentRotation:", camera::active().parentRotationAffectCurrentRotation); });
        m_input->action("f9").name("zoom").hold([this]{ camera::active().zoomByFov = not camera::active().zoomByFov; });
        m_input->action("\\").hold([this]{
            if(m_input->currentKey.onHoldTime > 700 and m_freeView){
                m_input->currentKey.release = true;
                // m_scene->freeCams.getController().changeMode();
                console.log("state changed");
            }
            else if(m_input->currentKey.onHoldTime > 700 and not m_freeView){
                // TODO: create freecam from current camera
            }
         });
        m_input->action("P").on([this]{
            if(m_player and not m_player->hasFocus()){
                m_player->focusOn();
                m_freeView = false;
            }
            else if(m_player and m_player->hasFocus()){
                m_player->focusOff();
                m_freeView = true;
                m_scene->freeCams.focus();
            }
         });
        m_input->action("[").on([this]{ if(m_freeView) m_scene->freeCams.prev(); });
        m_input->action("]").on([this]{ if(m_freeView) m_scene->freeCams.next(); });

        m_input->action("MousePosition").on([this](float x, float y){
            m_mousePos = glm::vec2(x,y);
            m_pointerInfo.screenPosition = m_mousePos;
            m_mouseSampler->samplePosition = m_mousePos;
        });
        m_input->action("MouseMove").on([this](float x, float y){
            m_mouseTranslation = glm::vec2(x,y) * m_window.size * 2.f;
            m_mouseTranslationNormalized = glm::vec2(x,y);
            // risk of camera change?
            // camera::active().pointerMovement.horizontal = x;
            // camera::active().pointerMovement.vertical = y;
        });
        m_input->activate();
    }

Playground::~Playground(){
    m_input->deactivate();
}
void Playground::update(float dt){
    console_prefix("Update");
    if(m_player) m_player->updateGraphic(dt);
    for(auto & bot : m_scene->m_friendlyBots){
        bot->updateGraphic(dt);
    }
    for(auto & bot : m_scene->m_hostileBots){
        bot->updateGraphic(dt);
    }
}
void Playground::updateWithHighPrecision(float dt){
    console_prefix("Precise Update");
    m_physics->update(dt/1000.f);

    auto& currentCamera = camera::active();
    if(m_freeView and not m_scene->freeCams.getController().hasFocus()){ // * I hope player doesn't have control over it's cameras
        m_scene->freeCams.focus();
    }

    if(m_player) m_player->update(dt);
    for(auto & bot : m_scene->m_friendlyBots){
        bot->update(dt);
    }
    for(auto & bot : m_scene->m_hostileBots){
        bot->update(dt);
    }
    m_scene->update(dt, currentCamera);

    if(currentCamera.reqiuresToFocusOnPoint){
        // pointer.setFromWorldPosition(worldPointToFocusOn or rotateAroundThisPoint or worldPointToFocusOnWhenSteady, currentCamera.orientation);
        // pointer.move(pointer.laseMoveInPx);
        // oblicza pozycję w którą przesunać kursor, wykonuje przesuniecię, dzięki czemu zmieni się kierunek patrzenia kamery
        // może będzie wystarczająco gładko chodzić
    }

    // if(camera::controlBasedOnVectors){
        /*
            nie iwem jeszcze
        */
        // currentCamera.directionToAlignCamera(glm::normalize(mouseSampler->position - currentCamera.eyePosition()));
    // }
    // else if(camera::controlBasedOnEulers){
        // todo: zapętlanie pozycji myszy
    if(m_freeView and camera::active().rotateAroundThisPoint or not m_freeView){
        currentCamera.pointerMovement.horizontal = m_mouseTranslationNormalized.x * dt/frameMs;
        currentCamera.pointerMovement.vertical = m_mouseTranslationNormalized.y * dt/frameMs;
    }
    // }

    if(m_freeView) currentCamera.update(dt);
    // todo:? wtf? przecież kamery z playera zgłupieją
    // for(auto &cam : CameraController::listOf){
    //     cam->update(dt);
    // }
    m_mouseTranslationNormalized = {};
}

void Playground::renderProcedure(GraphicEngine& renderer){
    console_prefix("Rendering");
    RenderDataCollector::collectCamera(camera::active());
    RenderDataCollector::collectWindow(m_window);
    RenderDataCollector::collectTime(FrameTime::deltaf, FrameTime::miliseconds);

    renderer.context->uploadUniforms();

    renderer.context->beginFrame();
    renderer.context->setupFramebufferForGBufferGeneration();
    // renderer.utils->drawBackground("nebula2");
    renderer.sceneRenderer->renderScene(*m_scene, camera::active());

    renderer.details->executeAtEndOfFrame();

    renderer.gBufferSamplers->sampleGBuffer(camera::active());

    m_pointerInfo.worldPosition = m_mouseSampler->position;

    renderer.effects->SSAO(camera::active());

    // renderer.shadowCaster->updateShadows();

    renderer.context->setupFramebufferForLighting();
    renderer.lightRendering->lightPass(*m_scene, camera::active());
    renderer.lightRendering->compose(camera::active());

    // renderer.effects->scattering(*m_scene, CameraController::active());
    renderer.effects->sky(*m_scene, camera::active());

    renderer.context->setupFramebufferForLDRProcessing();
    renderer.effects->toneMapping();
    renderer.effects->FXAA();
    // renderer.effects->chromaticDistortion(glm::vec3(1,0.5,0));
    renderer.effects->filmGrain();

    renderer.context->tex.gbuffer.color.genMipmaps();

    // renderer.uiRender->render(m_ui.getToRender());
    renderer.context->endFrame();
}
#include "AiControl.hpp"
#include "Pathfinder.hpp"
#include "NoPathfinder.hpp"
Scene& Playground::loadScene(const std::string& configName){
    m_scene->load(configName);
    return *m_scene;
}
void Playground::spawnPlayer(const std::string& configName, const glm::mat4& spawnPoint){
    VehicleAssembler builder(configName, *m_physics, m_window.camFactory);
    auto& vehicle = m_vehicles.emplace_back(builder.build(spawnPoint));

    m_player = std::make_shared<Player>(m_input->getDispatcher(), *vehicle);
    // m_player->focusOn();
    // m_freeView = false;
}
void Playground::spawnBot(const std::string& configName, const glm::mat4& spawnPoint, Context& renderingContext){

    VehicleAssembler builder(configName, *m_physics, m_window.camFactory);
    auto& vehicle = m_vehicles.emplace_back(builder.build(spawnPoint));

    auto& bot = *m_scene->m_friendlyBots.emplace_back(
        std::make_shared<AI>(std::make_unique<AiControlViaInput>(m_input->getDispatcher(), m_pointerInfo),
                             std::make_unique<NoPathfinder>(),
                             *vehicle));
}
void Playground::spawnHostileBot(const std::string& configName, const glm::mat4& spawnPoint){

    VehicleAssembler builder(configName, *m_physics, m_window.camFactory);
    auto& vehicle = m_vehicles.emplace_back(builder.build(spawnPoint));

    auto& bot = *m_scene->m_hostileBots.emplace_back(
        std::make_shared<AI>(std::make_unique<AiSelfControl>(),
                             std::make_unique<NoPathfinder>(),
                             *vehicle));
}
