#include "core.hpp"
#include "Playground.hpp"
#include "AI.hpp"
#include "AiControl.hpp"
#include "Context.hpp"
#include "Details.hpp"
#include "Effects.hpp"
#include "FrameTime.hpp"
#include "GBufferSampler.hpp"
#include "GraphicEngine.hpp"
#include "LightRendering.hpp"
#include "NoPathfinder.hpp"
#include "Pathfinder.hpp"
#include "PhysicalWorld.hpp"
#include "Player.hpp"
#include "PlaygroundEvents.hpp"
#include "RendererUtils.hpp"
#include "Scene.hpp"
#include "SceneRenderer.hpp"
#include "Settings.hpp"
#include "Texture.hpp"
#include "VehicleAssembler.hpp"
#include "Window.hpp"
#include "camera-controller.hpp"
#include "input-dispatcher.hpp"
#include "input-user-pointer.hpp"
#include "input.hpp"
#include "visuals-csm.hpp"
#include "visuals-prepared-scene.hpp"

Playground::Playground(Imgui& ui, InputDispatcher& inputDispatcher, Window& window, InputUserPointer& inputUserPointer)
    : m_input(inputDispatcher.createNew("Playground")),
      m_physics(std::make_unique<PhysicalWorld>()),
      m_window(window),
      m_inputUserPointer(inputUserPointer),
      m_scene(std::make_unique<Scene>(*m_physics, m_window.camFactory)),
      m_mouseSampler(std::make_unique<GBufferSampler>()) {
    float defaultCameraVelocity = 2;
    float preciseCameraVelocity = 0.1;

    m_mouseSampler->samplePosition = glm::vec2(0, 0);
    // todo: ustawić stan wskaźnika na odpowiedni

    m_input->action("esc").on([] { event<ExitPlayground>(); });
    m_input->action("f12").on([this] { camera::active().printDebug(); });
    m_input->action("+").on([this] { camera::active().input.zoom = +1.5; });
    m_input->action("-").on([this] { camera::active().input.zoom = -1.5; });
    m_input->action("scrollUp").on([=] {
        m_scene->freeCams.getController().input.worldPointToZoom = m_mouseSampler->position;
        camera::active().input.zoom = 1;
    });
    m_input->action("scrollDown").on([=] {
        m_scene->freeCams.getController().input.worldPointToZoom = m_mouseSampler->position;
        camera::active().input.zoom = -1;
    });
    m_input->action("RMB")
        .on([this] {
            // TODO: move freecam, in a way that mouse world position is preserved
            auto& cam = camera::active();
            // todo: cam.lock(); -> cannot be changed

            if(cam.userPointerMode != camera::PointerMode::Free)
                cam.input.worldPointToFocusOn = m_mouseSampler->position;
            else
                cam.input.worldPointToPivot = m_mouseSampler->position;

            cam.previousUserPointerMode = cam.userPointerMode;
            cam.userPointerMode = camera::PointerMode::OnPoint;
        })
        .off([this] {
            auto& cam = camera::active();
            cam.input.worldPointToFocusOn.reset();
            cam.input.worldPointToPivot.reset();
            cam.userPointerMode = cam.previousUserPointerMode;
            // todo: cam.unlock(); -> cann be changed
        });
    m_input->action("LMB")
        .on([this] {
            m_cameraRotate = true;
            if(camera::active().setup.isFreecam)
                camera::active().input.worldPointToPivot = m_mouseSampler->position;
        })
        .off([this] {
            m_cameraRotate = false;
            if(camera::active().setup.isFreecam)
                camera::active().input.worldPointToPivot.reset();
        });
    m_input->action("Q").on([this] { camera::active().input.pointer.roll = -15 * toRad; });
    m_input->action("E").on([this] { camera::active().input.pointer.roll = +15 * toRad; });
    m_input->action("ctrl").on([this] { m_inputUserPointer.showSystemCursor(); }).off([this] {
        m_inputUserPointer.hideSystemCursor();
    });
    m_input->action("shift").on([this] { shiftMode = true; }).off([this] { shiftMode = false; });

    m_input->action("W").hold([this] { camera::active().input.velocity.z = cameraSpeed(); });
    m_input->action("S").hold([this] { camera::active().input.velocity.z = -cameraSpeed(); });
    m_input->action("A").hold([this] { camera::active().input.velocity.x = -cameraSpeed(); });
    m_input->action("D").hold([this] { camera::active().input.velocity.x = cameraSpeed(); });
    m_input->action("Z").hold([this] { camera::active().input.velocity.y = cameraSpeed(); });
    m_input->action("X").hold([this] { camera::active().input.velocity.y = -cameraSpeed(); });
    m_input->action("f5").name("global direction").hold([this] {
        camera::active().setup.inLocalSpace = false;
        camera::active().setup.addRotationToTarget = false;
        camera::active().setup.addInclinationToTarget = false;
        camera::active().userPointerMode == camera::PointerMode::Centered;
    });
    m_input->action("f6").name("local direction").hold([this] {
        camera::active().setup.inLocalSpace = true;
        camera::active().setup.addRotationToTarget = false;
        camera::active().setup.addInclinationToTarget = false;
        camera::active().userPointerMode == camera::PointerMode::Deviation;
    });
    m_input->action("f7").name("enable stabilization").hold([this] {
        camera::active().setup.alignHorizontally = !camera::active().setup.alignHorizontally;
        console.log("stabilization:", camera::active().setup.alignHorizontally ? "enabled" : "disabled");
    });
    m_input->action("f9").name("zoom").hold([this] {
        camera::active().setup.zoomMode = not camera::active().setup.zoomMode;
        console.log("zoomMode by", camera::active().setup.zoomMode ? "offset" : "fov");
    });
    m_input->action("\\").hold([this] {
        if(m_input->currentKey.onHoldTime > 700 and camera::active().setup.isFreecam) {
            m_input->currentKey.release = true;
            // m_scene->freeCams.getController().changeMode();
            console.log("state changed");
        }
        else if(m_input->currentKey.onHoldTime > 700 and not camera::active().setup.isFreecam) {
            // TODO: create freecam from current camera
        }
    });
    m_input->action("P").on([this] {
        if(m_player and not m_player->hasFocus()) {
            if(camera::active().userPointerMode == camera::PointerMode::Free) {
                m_inputUserPointer.hideSystemCursor();
            }
            m_player->focusOn();
        }
        else if(m_player and m_player->hasFocus()) {
            m_player->focusOff();
            m_scene->freeCams.focus();
            if(camera::active().userPointerMode == camera::PointerMode::Free) {
                m_inputUserPointer.showSystemCursor();
            }
        }
    });
    m_input->action("[").on([this] {
        if(camera::active().setup.isFreecam) {
            auto& prevCam = camera::active();

            m_scene->freeCams.prev();

            if(camera::active().userPointerMode == camera::PointerMode::Free
               and prevCam.userPointerMode != camera::PointerMode::Free) {
                m_inputUserPointer.showSystemCursor();
            }
            else if(camera::active().userPointerMode != camera::PointerMode::Free
                    and prevCam.userPointerMode == camera::PointerMode::Free) {
                m_inputUserPointer.hideSystemCursor();
            }
        }
    });
    m_input->action("]").on([this] {
        if(camera::active().setup.isFreecam) {
            auto& prevCam = camera::active();

            m_scene->freeCams.next();

            if(camera::active().userPointerMode == camera::PointerMode::Free
               and prevCam.userPointerMode != camera::PointerMode::Free) {
                m_inputUserPointer.showSystemCursor();
            }
            else if(camera::active().userPointerMode != camera::PointerMode::Free
                    and prevCam.userPointerMode == camera::PointerMode::Free) {
                m_inputUserPointer.hideSystemCursor();
            }
        }
    });
    m_input->activate();
}

Playground::~Playground() {
    m_input->deactivate();
}
void Playground::update(float dt) {
    console_prefix("Update");

    camera::makeSnapshot();

    if(m_player)
        m_player->updateGraphic(dt);
    updateCamera(dt);
    for(auto& bot : m_scene->m_friendlyBots) { bot->updateGraphic(dt); }
    for(auto& bot : m_scene->m_hostileBots) { bot->updateGraphic(dt); }
}
void Playground::updateWithHighPrecision(float dt) {
    console_prefix("Precise Update");
    m_physics->update(dt / 1000.f);

    auto& currentCamera = camera::active();
    if(camera::active().setup.isFreecam
       and not m_scene->freeCams.getController().hasFocus()) { // * I hope player doesn't have control over it's cameras
        m_scene->freeCams.focus();
    }

    m_scene->updateWorld(dt);
    m_scene->updateNonPlayableObjects(dt, currentCamera);

    if(m_player)
        m_player->update(dt);
    for(auto& bot : m_scene->m_friendlyBots) { bot->update(dt); }
    for(auto& bot : m_scene->m_hostileBots) { bot->update(dt); }
    m_scene->updateLightsAndShadows(dt, currentCamera);
    m_scene->collectObjectForRendering(dt, currentCamera);

    m_mouseSampler->samplePosition = m_inputUserPointer.screenPosition();
}

void Playground::updateCamera(float dt) {
    auto& currentCamera = camera::active();

    auto pointerDelta = m_inputUserPointer.delta() * m_inputUserPointer.screenScale() * m_inputUserPointer.sensitivity;

    // for pointer rendering
    if(currentCamera.userPointerMode == camera::PointerMode::OnPoint) {
        //  set pointer on point on which camera is focused, point cannot be moved by mouse
        m_inputUserPointer.setFromWorldPosition(*currentCamera.input.worldPointToFocusOn, currentCamera.orientation);
    }
    else if(currentCamera.userPointerMode == camera::PointerMode::Deviation) {
        // todo: m_inputUserPointer.setFromGame();
    }
    else if(currentCamera.userPointerMode == camera::PointerMode::Centered) {
        m_inputUserPointer.setCentered();
    }
    // ten tu chyba nie działa, free mode uzywa pointera wystemowego
    else if(currentCamera.userPointerMode == camera::PointerMode::Free) {
        m_inputUserPointer.setFromGame(m_inputUserPointer.screenPosition());
    }

    // todo: zapętlanie pozycji myszy
    // to rotate camera and rotate freecam around point, allows to move pointer freely
    if((currentCamera.setup.isFreecam
        and (currentCamera.input.worldPointToPivot or currentCamera.userPointerMode != camera::PointerMode::Free))
       or not currentCamera.setup.isFreecam) {
        currentCamera.input.pointer.horizontal = pointerDelta.x * dt / frameMs;
        currentCamera.input.pointer.vertical = pointerDelta.y * dt / frameMs;
    }

    currentCamera.update(dt);
}

void Playground::renderProcedure(GraphicEngine& renderer) {
    console_prefix("Rendering");
    // visuals::preparedScene.collectCamera(camera::active());
    visuals::preparedScene.collectWindow(m_window);
    visuals::preparedScene.collectTime(FrameTime::deltaf, FrameTime::miliseconds);

    renderer.context->uploadUniforms();
    renderer.context->beginFrame();

    renderer.csm->prepare(visuals::preparedScene.mainLight, visuals::preparedScene.cameraOfThisFrame);
    renderer.csm->render();
    renderer.csm->finish();

    renderer.context->setupFramebufferForGBufferGeneration();
    // renderer.utils->drawBackground("nebula2");
    renderer.sceneRenderer->renderScene(*m_scene, visuals::preparedScene.cameraOfThisFrame);

    renderer.details->executeAtEndOfFrame();

    renderer.gBufferSamplers->sampleGBuffer(visuals::preparedScene.cameraOfThisFrame);

    m_inputUserPointer.world.position = m_mouseSampler->position;
    m_inputUserPointer.world.normal = m_mouseSampler->normal;

    renderer.effects->SSAO(visuals::preparedScene.cameraOfThisFrame);

    renderer.context->setupFramebufferForLighting();
    renderer.lightRendering->lightPass(*m_scene, visuals::preparedScene.cameraOfThisFrame);
    renderer.lightRendering->compose(visuals::preparedScene.cameraOfThisFrame);

    // renderer.effects->scattering(*m_scene, CameraController::active());
    renderer.effects->sky(*m_scene, visuals::preparedScene.cameraOfThisFrame);

    renderer.context->setupFramebufferForLDRProcessing();
    renderer.effects->toneMapping();
    renderer.effects->FXAA();
    // renderer.effects->chromaticDistortion(glm::vec3(1,0.5,0));
    renderer.effects->filmGrain();

    renderer.context->tex.gbuffer.color.genMipmaps();

    // renderer.uiRender->render(m_ui.getToRender());
    renderer.context->endFrame();
}

Scene& Playground::loadScene(const std::string& configName) {
    m_scene->load(configName);
    return *m_scene;
}
void Playground::spawnPlayer(const std::string& configName, const glm::mat4& spawnPoint) {
    VehicleAssembler builder(configName, *m_physics, m_window.camFactory);
    auto& vehicle = m_vehicles.emplace_back(builder.build(spawnPoint));

    m_player = std::make_shared<Player>(m_input->getDispatcher(), *vehicle, m_inputUserPointer);
    m_player->focusOn();

    m_player->focusOff();
    m_scene->freeCams.focus();
}
void Playground::spawnBot(const std::string& configName, const glm::mat4& spawnPoint, Context& renderingContext) {
    VehicleAssembler builder(configName, *m_physics, m_window.camFactory);
    auto& vehicle = m_vehicles.emplace_back(builder.build(spawnPoint));

    auto& bot = *m_scene->m_friendlyBots.emplace_back(std::make_shared<AI>(
        std::make_unique<AiControlViaInput>(m_input->getDispatcher()), std::make_unique<NoPathfinder>(), *vehicle));
}
void Playground::spawnHostileBot(const std::string& configName, const glm::mat4& spawnPoint) {
    VehicleAssembler builder(configName, *m_physics, m_window.camFactory);
    auto& vehicle = m_vehicles.emplace_back(builder.build(spawnPoint));

    auto& bot = *m_scene->m_hostileBots.emplace_back(
        std::make_shared<AI>(std::make_unique<AiSelfControl>(), std::make_unique<NoPathfinder>(), *vehicle));
}
