#include <GLFW/glfw3.h>

#include "Includes.hpp"
#include "App.hpp"
// #include "Events.hpp"
#include "EventProcessor.hpp"
#include "GameState.hpp"
#include "Settings.hpp"
// #include "GraphicEngine.hpp"
#include "KeyState.hpp" // TODO: Remove
#include "Logging.hpp"
#include "PerfTimers.hpp"
#include "ResourceLoader.hpp"
#include "Timer.hpp"
#include "Window.hpp"
#include "Yaml.hpp"
#include "DebugScreen.hpp"
// #include "PhysicsWorld.hpp"
// #include "ParticleProcessor.hpp"

#include "UIUpdater.hpp"

bool CLOG_SPECIAL_VALUE = false;
bool CLOG_SPECIAL_VALUE_2 = false;
bool CLOG_SPECIAL_VALUE_3 = false;
bool ONCE_IN_FRAME = false;
bool quit = false;
bool CURSOR_DISABLED = true;
bool TAKE_SCREENSHOT = false;
bool HIDE_UI = false;
bool ALT_MODE = false;
bool SHIFT_MODE = false;
bool CTRL_MODE = false;

glm::vec2 App::lastCursorPos;
App* App::self = nullptr;

App::App() : inputContext(inputHandler), settings(std::make_unique<Settings>()){
    self = this;
}
App::~App(){
    log("~App");
};

bool App::initialize(){
    log("--initializing main systems");

    settings->load();

    window = std::make_unique<Window>(*this);
    if(not window->init()) return false;

    glfwSetWindowUserPointer(window->window, this);

    initializeInputHandler();

    uiUpdater = std::make_unique<UI::Updater>(*window);
    ui = uiUpdater->createUi();
    // graphicEngine = std::make_unique<GraphicEngine>(*window, *uiUpdater);
    // debugScreen = std::make_unique<DebugScreen>();
    // debugScreen->init();

    if(CURSOR_DISABLED){
        glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        KeyState::mouseReset = true;
        // lastCursorPos = glm::vec2(window->size.x/2, window->size.y/2);
        // glfwSetCursorPos(window->window, window->size.x/2, window->size.y/2);
    }
    // physics = std::make_unique<PhysicsWorld>();
    // physics->init();
    eventProcessor = std::make_unique<EventProcessor>(*this);
    // particleProcessor = std::make_unique<ParticleProcessor>(*physics);

    return true;
}
void App::initializeInputHandler(){
    inputContext.setAction("LMB", "default LMB", []{
            KeyState::lClicked = true;
            KeyState::lClick = true;
        }, []{
            KeyState::lClicked = false;
        });
    inputContext.setAction("MMB", "default LMB", []{
            KeyState::rClicked = true;
            KeyState::rClick = true;
        }, []{
            KeyState::rClicked = false;
        });
    inputContext.setAction("RMB", "default LMB", []{
            KeyState::mClicked = true;
            KeyState::mClick = true;
        }, []{
            KeyState::mClicked = false;
        });

    inputContext.setAction("printScreen", "", []{ TAKE_SCREENSHOT = true; });
    inputContext.setAction("alt", "", []{ ALT_MODE = true; }, []{ ALT_MODE = false; });
    inputContext.setAction("shift", "", []{ SHIFT_MODE = true; }, []{ SHIFT_MODE = false; });
    inputContext.setAction("ctrl", "", []{ CTRL_MODE = true; }, []{ CTRL_MODE = false; });
    inputContext.setAction("f4", "exit", [this]{ if(ALT_MODE) quit = true; });
    inputContext.setAction("esc", "exit", [this]{ quit = true; });
    inputContext.setAction("f1", "help", []{ log("Helpful message"); });
    inputContext.setAction("f10", "screenshot", []{ TAKE_SCREENSHOT = true; });
    inputContext.setAction("f11", "debug", []{
            CLOG_SPECIAL_VALUE_3 != CLOG_SPECIAL_VALUE_3;
        });
    inputContext.setAction("f12", "debug 2", []{
            CLOG_SPECIAL_VALUE = true;
            CLOG_SPECIAL_VALUE_2 = true;
            GpuTimerScoped::print();
        }, []{ CLOG_SPECIAL_VALUE_2 = false; });
    inputContext.setAction("H", "Hide UI", []{ HIDE_UI = !HIDE_UI; });
    inputContext.setAction("R", "Reload Shaders", []{
            ResourceLoader loader;
            log("Reloading shaders");
            Yaml shadersToReload("../ShadersToReload.yml");
            for(auto &it : shadersToReload){
                loader.reloadShader(it.string());
            }});
    inputContext.setAction("ctrl", "hide ui", [this]{
            if(CURSOR_DISABLED){
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                KeyState::mouseReset = false;
            }
        }, [this]{
            if(CURSOR_DISABLED){
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                KeyState::mouseReset = true;
            }
            });
    inputContext.setAction("f2", "hide cursor", [this]{
            if(CURSOR_DISABLED){
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                KeyState::mouseReset = true;
            } else {
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                KeyState::mouseReset = false;
            }
            CURSOR_DISABLED = !CURSOR_DISABLED;
        }, []{});

    inputContext.activate();
}

void App::setCommonCallbacks(){
    log("--setting up callbacks");
    glfwSetScrollCallback(window->window, &App::scrollCallback);
    glfwSetKeyCallback(window->window, &App::keyCallback);
    glfwSetMouseButtonCallback(window->window, &App::mouseButtonCallback);
    glfwSetCursorPosCallback(window->window, &App::cursorPosCallback);
    glfwSetWindowCloseCallback(window->window, &App::exitCallback);
    glfwSetErrorCallback(&App::errorCallback);
}

bool App::loadResources(){
    log("--loading resources");

    Yaml resources("../data/GameResources.yml");

    ResourceLoader loader;
    loader.loadShaders();
    loader.loadResources(resources);

    return true;
}
void App::run(){
    log("--starting main loop");

    Timer<float, 1000, 1> loopDeltaTime;
    Timer<u32, 1000, 1> msecLoopDeltaTime;
    float fixedStepLoopAccumulator = 20;
    float fixedStepLoopStep = 1000.0/60.0/4.0;

    glfwPollEvents();
    eventProcessor->process();
    while(not quit){
        CPU_SCOPE_TIMER("Main loop update");
        auto dt = loopDeltaTime();
        auto msdt = msecLoopDeltaTime();
        CLOG_SPECIAL_VALUE = false;
        KeyState::mouseTranslation = glm::vec2(0);
        KeyState::mouseTranslationNormalized = glm::vec2(0);

        uiUpdater->update(dt);
        uiUpdater->begin();

        glfwPollEvents();
        inputHandler.refresh();

        fixedStepLoopAccumulator += dt;
        while(fixedStepLoopAccumulator > 0.f and not quit){
            fixedStepLoopAccumulator -= fixedStepLoopStep;
            ONCE_IN_FRAME = false;
        }
        ONCE_IN_FRAME = true;

        if(gameState) gameState->updateWithHighPrecision(dt);
        if(gameState) gameState->update(dt);

        eventProcessor->process();
        // particleProcessor->update(dt);

        // debugScreen->show(*ui, true);
        // debugScreen->options(*ui);
        uiUpdater->end();
        render();
    }
}
void App::render(){
    clog("--render");

    // if(game) game->render(*graphicEngine);

    if(TAKE_SCREENSHOT){
        // graphicEngine->takeScreenShot();
        TAKE_SCREENSHOT = false;
    }
    glfwSwapBuffers(window->window);
}
void App::finish(){
    log("--finishing");
    // graphicEngine.reset();
    // game.reset();
    window.reset();
}

void App::setGameState(std::shared_ptr<GameState> p_gameState){
    // cleanup after previous state?
    // cleanup resources, czy też zrobi się to automatycznie? automat będzie lepszy
    gameState = p_gameState;
}

void App::scrollCallback(GLFWwindow *w, double dx, double dy){
    self->inputHandler.scrollCallback(dx, dy);
}
void App::keyCallback(GLFWwindow *w, int key, int scancode, int action, int mods){
    self->inputHandler.keyCallback(key, action, mods);
}
void App::mouseButtonCallback(GLFWwindow *w, int button, int action, int mods){
    // if(not KeyState::mouseReset) self->uiUpdater->setMouseAction(button, action);
    self->inputHandler.mouseButtonCallback(button, action, mods);
}
void App::cursorPosCallback(GLFWwindow *w, double xpos, double ypos){
    // if(not KeyState::mouseReset) self->uiUpdater->setMousePosition(xpos, ypos);

    auto size = self->window->size;

    auto mp = lastCursorPos;
    if(not KeyState::mouseReset) KeyState::mousePosition = glm::vec2(xpos, size.y -  ypos);
    else KeyState::mousePosition = glm::vec2(-500);

    lastCursorPos = glm::vec2(xpos, size.y -  ypos);
    KeyState::mouseTranslation = lastCursorPos - mp;
    KeyState::mouseTranslation.y *= -1;
    KeyState::mouseTranslationNormalized = KeyState::mouseTranslation / size * 0.5f;
    self->inputHandler.mousePosition(KeyState::mousePosition.x, KeyState::mousePosition.y);
    self->inputHandler.mouseMovement(KeyState::mouseTranslation.x, KeyState::mouseTranslation.y);
}
void App::exitCallback(GLFWwindow *w){
    self->quit = true;
}
void App::errorCallback(int errorCode, const char* description){
    error("[GLFW]", description);
}
