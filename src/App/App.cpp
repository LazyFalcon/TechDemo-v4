#include "core.hpp"
#include <GLFW/glfw3.h>

#include "App.hpp"
#include "AudioLibrary.hpp"
#include "DebugScreen.hpp"
#include "EventProcessor.hpp"
#include "GameState.hpp"
#include "GraphicEngine.hpp"
#include "input-dispatcher.hpp"
#include "input.hpp"
#include "Logging.hpp"
#include "PerfTimers.hpp"
#include "ResourceLoader.hpp"
#include "Settings.hpp"
#include "Timer.hpp"
#include "ui.hpp"
#include "Window.hpp"
#include "Yaml.hpp"

bool CLOG_SPECIAL_VALUE = false;
bool CLOG_SPECIAL_VALUE_2 = false;
bool CLOG_SPECIAL_VALUE_3 = false;
bool ONCE_IN_FRAME = false;
bool quit = false;
bool CURSOR_DISABLED = false;
bool TAKE_SCREENSHOT = false;
bool HIDE_UI = false;
bool ALT_MODE = false;
bool SHIFT_MODE = false;
bool CTRL_MODE = false;

App* App::self = nullptr;

App::App() : inputDispatcher(std::make_unique<InputDispatcher>()), input(inputDispatcher->createNew("App")), settings(std::make_unique<Settings>()){
    self = this;
}
App::~App(){
    log("~App");
};

bool App::initialize(){
    log("--initializing main systems");
    audio = std::make_unique<AudioLibrary>();
    settings->load();

    window = std::make_unique<Window>(*this);
    if(not window->init()) return false;

    glfwSetWindowUserPointer(window->window, this);

    initializeInputDispatcher();

    // uiUpdater = std::make_unique<UI::Updater>(*window);
    imgui = std::make_unique<Imgui>(window->size.x, window->size.y);
    graphicEngine = std::make_unique<GraphicEngine>(*window);
    // debugScreen = std::make_unique<DebugScreen>();
    // debugScreen->init();

    if(CURSOR_DISABLED){
        glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // KeyState::mouseReset = true;
        // lastCursorPos = glm::vec2(window->size.x/2, window->size.y/2);
        // glfwSetCursorPos(window->window, window->size.x/2, window->size.y/2);
    }
    // physics = std::make_unique<PhysicalWorld>();
    // physics->init();
    eventProcessor = std::make_unique<EventProcessor>(*this);
    // particleProcessor = std::make_unique<ParticleProcessor>(*physics);

    return true;
}
void App::initializeInputDispatcher(){
    input->action("LMB").on([this]{
            imgui->input.lmbOn();
            imgui->panelInput.lmbOn();
        }).off([this]{
            imgui->input.lmbOff();
            imgui->panelInput.lmbOff();
        });
    input->action("MMB").on([this]{});
    input->action("RMB").on([this]{
            imgui->input.rmbOn();
            imgui->panelInput.rmbOn();
        }).off([this]{
            imgui->input.rmbOff();
            imgui->panelInput.rmbOff();
        });

    input->action("printScreen").on([]{ TAKE_SCREENSHOT = true; });
    input->action("alt").on([]{ ALT_MODE = true; }).off([]{ ALT_MODE = false; });
    input->action("shift").on([]{ SHIFT_MODE = true; }).off([]{ SHIFT_MODE = false; });
    input->action("ctrl").on([]{ CTRL_MODE = true; }).off([]{ CTRL_MODE = false; });
    input->action("alt-f2").on([this]{ quit = true; });
    // input->action("esc", "exit", [this]{ quit = true; });
    input->action("f1").on([]{ log("Helpful message"); });
    input->action("f10").on([]{ TAKE_SCREENSHOT = true; });
    input->action("f11").on([]{
            CLOG_SPECIAL_VALUE_3 != CLOG_SPECIAL_VALUE_3;
        });
    input->action("f12").on([]{
            CLOG_SPECIAL_VALUE = true;
            CLOG_SPECIAL_VALUE_2 = true;
            GpuTimerScoped::print();
        }).off([]{ CLOG_SPECIAL_VALUE_2 = false; });
    input->action("H").on([]{ HIDE_UI = !HIDE_UI; });
    input->action("R").on([]{
            ResourceLoader loader;
            log("Reloading shaders");
            Yaml shadersToReload(rootPath + "ShadersToReload.yml");
            for(auto &it : shadersToReload){
                loader.reloadShader(it.string());
            }});
    input->action("ctrl").on([this]{
            if(CURSOR_DISABLED){
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                // KeyState::mouseReset = false;
            }})
            .off([this]{
            if(CURSOR_DISABLED){
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                // KeyState::mouseReset = true;
            }});
    input->action("f2").on([this]{
            if(CURSOR_DISABLED){
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                // KeyState::mouseReset = true;
            } else {
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                // KeyState::mouseReset = false;
            }
            CURSOR_DISABLED = !CURSOR_DISABLED;
        });
    input->action("MousePosition").on([this](float x, float y){
        imgui->input.mousePos = glm::vec2(x,y);
        imgui->panelInput.mousePos = glm::vec2(x,y);
    });
    input->action("MouseMove").on([this](float x, float y){
        imgui->input.mouseTranslation = glm::vec2(x,y) * window->size*2.f;
        imgui->panelInput.mouseTranslation = glm::vec2(x,y) * window->size*2.f;
    });

    input->forward([this](const std::string& action){
        imgui->pressedKey = action;
    });

    input->activate();
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

    Yaml resources(dataPath + "GameResources.yml");

    ResourceLoader loader;
    loader.loadShaders();
    loader.loadResources(resources);

    return true;
}
void App::run() try {
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

        imgui->restart();

        inputDispatcher->heldUpKeys();
        glfwPollEvents();

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
        // uiUpdater->end();
        render();
    }
}
catch(const std::runtime_error& err){
    error(err.what());
}

void App::render(){
    clog("--render");

    if(gameState) gameState->renderProcedure(*graphicEngine);

    if(TAKE_SCREENSHOT){
        // graphicEngine->takeScreenShot();
        TAKE_SCREENSHOT = false;
    }
    glfwSwapBuffers(window->window);
}
void App::finish(){
    log("--finishing");
    graphicEngine.reset();
    // game.reset();
    window.reset();
}

void App::setGameState(std::shared_ptr<GameState> p_gameState){
    // cleanup after previous state?
    // cleanup resources, czy też zrobi się to automatycznie? automat będzie lepszy
    gameState = p_gameState;
}


void App::hideMouse(){
    CURSOR_DISABLED = false;
}
void App::showMouse(){
    CURSOR_DISABLED = true;
}

void App::scrollCallback(GLFWwindow *w, double dx, double dy){
    self->inputDispatcher->scrollCallback(dx, dy);
}
void App::keyCallback(GLFWwindow *w, int key, int scancode, int action, int mods){
    self->inputDispatcher->keyCallback(key, action, mods);
}
void App::mouseButtonCallback(GLFWwindow *w, int button, int action, int mods){
    // if(not KeyState::mouseReset) self->uiUpdater->setMouseAction(button, action);
    self->inputDispatcher->mouseButtonCallback(button, action, mods);
}
void App::cursorPosCallback(GLFWwindow *w, double xpos, double ypos){
    // if(not KeyState::mouseReset) self->uiUpdater->setMousePosition(xpos, ypos);
    auto size = self->window->size;

    auto last = self->lastCursorPos;
    self->lastCursorPos = glm::vec2(xpos, size.y -  ypos);
    float dx = (self->lastCursorPos.x - last.x) / size.x * 0.5f;
    float dy = -(self->lastCursorPos.y - last.y) / size.y * 0.5f;

    float x = xpos;
    float y = size.y - ypos;

    self->inputDispatcher->mousePosition(x, y);
    self->inputDispatcher->mouseMovement(dx, dy);
}
void App::exitCallback(GLFWwindow *w){
    self->quit = true;
}
void App::errorCallback(int errorCode, const char* description){
    error("[GLFW]", description);
}
