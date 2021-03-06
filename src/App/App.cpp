#include "core.hpp"
#include "App.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include "AudioLibrary.hpp"
#include "DebugScreen.hpp"
#include "EventProcessor.hpp"
#include "FrameTime.hpp"
#include "GameState.hpp"
#include "GraphicEngine.hpp"
#include "Logger.hpp"
#include "PerfTimers.hpp"
#include "ResourceLoader.hpp"
#include "Settings.hpp"
#include "Timer.hpp"
#include "Window.hpp"
#include "Yaml.hpp"
#include "camera-controller.hpp"
#include "input-dispatcher.hpp"
#include "input-user-pointer.hpp"
#include "input.hpp"
#include "ui.hpp"
#include "visuals-prepared-scene.hpp"


bool CLOG_SPECIAL_VALUE = false;
bool CLOG_SPECIAL_VALUE_2 = false;
bool CLOG_SPECIAL_VALUE_3 = false;
bool ONCE_IN_FRAME = false;
bool quit = false;
bool TAKE_SCREENSHOT = false;
bool HIDE_UI = false;
bool ALT_MODE = false;
bool SHIFT_MODE = false;
bool CTRL_MODE = false;

uint g_frameNumber;

void incrFrame() {
    ++g_frameNumber;
}

uint frame() {
    return g_frameNumber;
}

App* App::self = nullptr;

App::App()
    : inputDispatcher(std::make_unique<InputDispatcher>()),
      input(inputDispatcher->createNew("App")),
      settings(std::make_unique<Settings>()) {
    self = this;
}
App::~App() {
    console.log("~App");
};

bool App::initialize() {
    console.log("--initializing main systems");
    audio = std::make_unique<AudioLibrary>();
    settings->load();

    window = std::make_unique<Window>(*this);
    if(not window->init())
        return false;

    glfwSetWindowUserPointer(window->window, this);

    initializeInputDispatcher();

    // uiUpdater = std::make_unique<UI::Updater>(*window);
    imgui = std::make_unique<Imgui>(window->size.x, window->size.y);
    graphicEngine = std::make_unique<GraphicEngine>(*window);
    // debugScreen = std::make_unique<DebugScreen>();
    // debugScreen->init();

    eventProcessor = std::make_unique<EventProcessor>(*this);
    userPointer = std::make_unique<InputUserPointer>(*window, window->size);

    userPointer->showSystemCursor();
    return true;
}
// todo: move imgui input to proper game states
void App::initializeInputDispatcher() {
    input->action("LMB")
        .on([this] {
            if(userPointer->systemMode() or true) {
                imgui->input.lmbOn();
                imgui->panelInput.lmbOn();
            }
        })
        .off([this] {
            if(userPointer->systemMode() or true) {
                imgui->input.lmbOff();
                imgui->panelInput.lmbOff();
            }
        });
    input->action("MMB").on([this] {});
    input->action("RMB")
        .on([this] {
            if(userPointer->systemMode()) {
                imgui->input.rmbOn();
                imgui->panelInput.rmbOn();
            }
        })
        .off([this] {
            if(userPointer->systemMode()) {
                imgui->input.rmbOff();
                imgui->panelInput.rmbOff();
            }
        });

    input->action("printScreen").on([] { TAKE_SCREENSHOT = true; });
    input->action("alt").on([] { ALT_MODE = true; }).off([] { ALT_MODE = false; });
    input->action("shift").on([] { SHIFT_MODE = true; }).off([] { SHIFT_MODE = false; });
    input->action("ctrl").on([] { CTRL_MODE = true; }).off([] { CTRL_MODE = false; });
    input->action("alt-f2").on([this] { quit = true; });
    // input->action("esc", "exit", [this]{ quit = true; });
    input->action("f1").on([] { console.log("Helpful message"); });
    input->action("f10").on([] { TAKE_SCREENSHOT = true; });
    input->action("f12")
        .on([] {
            pmkLogger.printConditionalLogs();
            GpuTimerScoped::print();
        })
        .off([] { CLOG_SPECIAL_VALUE_2 = false; });
    input->action("H").on([] { HIDE_UI = !HIDE_UI; });
    input->action("R").on([] {
        ResourceLoader loader;
        console.log("Reloading shaders");
        Yaml shadersToReload(rootPath + "ShadersToReload.yml");
        for(auto& it : shadersToReload) {
            console.log("reloading shader: ", it.string());
            loader.reloadShader(it.string());
        }
    });
    input->action("MousePosition").on([this](float x, float y) {
        imgui->input.mousePos = glm::vec2(x, y);
        imgui->panelInput.mousePos = glm::vec2(x, y);
    });
    input->action("MouseMove").on([this](float x, float y) {
        imgui->input.mouseTranslation = glm::vec2(x, y);
        imgui->panelInput.mouseTranslation = glm::vec2(x, y);
    });

    input->forward([this](const std::string& action) { imgui->pressedKey = action; });

    input->activate();
}

void App::setCommonCallbacks() {
    console.log("--setting up callbacks");
    glfwSetScrollCallback(window->window, &App::scrollCallback);
    glfwSetKeyCallback(window->window, &App::keyCallback);
    glfwSetMouseButtonCallback(window->window, &App::mouseButtonCallback);
    glfwSetCursorPosCallback(window->window, &App::cursorPosCallback);
    glfwSetWindowCloseCallback(window->window, &App::exitCallback);
    glfwSetErrorCallback(&App::errorCallback);
}

bool App::loadResources() {
    console.log("--loading resources");

    Yaml resources(dataPath + "GameResources.yml");

    ResourceLoader loader;
    loader.loadShaders();
    loader.loadResources(resources);

    return true;
}

void App::run() try {
    using namespace std::chrono_literals;
    using clock = std::chrono::high_resolution_clock;
    console.log("--starting main loop");

    FrameTime::timeOnStart = clock::now();
    FrameTime::lastTimePoint = FrameTime::timeOnStart;
    std::chrono::nanoseconds timestep(16ms);
    std::chrono::nanoseconds lag(0ns);

    glfwPollEvents();
    eventProcessor->process();

    while(not quit) {
        std::cout << std::flush;
        GameState* currentGamestate = gameState.get();
        incrFrame();
        CPU_SCOPE_TIMER("Main loop update");
        auto deltaTime = clock::now() - FrameTime::lastTimePoint;
        FrameTime::lastTimePoint = clock::now();
        lag += std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTime);

        FrameTime::miliseconds = std::chrono::duration_cast<std::chrono::duration<uint, std::milli>>(
                                     FrameTime::lastTimePoint - FrameTime::timeOnStart)
                                     .count();
        FrameTime::nanoseconds = std::chrono::duration_cast<std::chrono::duration<uint, std::micro>>(
                                     FrameTime::lastTimePoint - FrameTime::timeOnStart)
                                     .count();
        FrameTime::delta = std::chrono::duration_cast<std::chrono::duration<uint, std::milli>>(deltaTime).count();
        FrameTime::deltaf = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(deltaTime).count();

        pmkLogger.startFrame();
        updateTimers(FrameTime::miliseconds);

        // TODO: renderData.storeCameraForFrameRendering();
        userPointer->update(FrameTime::deltaf);
        if(camera::hasActive())
            visuals::preparedScene.collectCamera(camera::active());

        imgui->restart();
        inputDispatcher->setTime(FrameTime::miliseconds);
        inputDispatcher->heldUpKeys();
        glfwPollEvents();
        if(gameState.get() == currentGamestate)
            eventProcessor->process();

        // considered as stable loop
        while(lag > timestep and not quit) {
            lag -= timestep;
            ONCE_IN_FRAME = false;
        }
        ONCE_IN_FRAME = true;

        // todo: consider combine it and put into stable loop
        console.flog("Frame time:", FrameTime::deltaf);
        if(gameState.get() == currentGamestate)
            gameState->updateWithHighPrecision(FrameTime::deltaf);
        if(gameState.get() == currentGamestate)
            gameState->update(FrameTime::deltaf);

        // particleProcessor->update(dt);

        // debugScreen->show(*ui, true);
        // debugScreen->options(*ui);
        // uiUpdater->end();
        if(gameState.get() == currentGamestate)
            render();
    }
}
catch(const std::runtime_error& err) {
    console.error(err.what());
}

void App::render() {
    console.clog("----------------------------------------------------------");
    console.clog("Frame number:", frame());

    if(gameState)
        gameState->renderProcedure(*graphicEngine);

    if(TAKE_SCREENSHOT) {
        graphicEngine->takeScreenShot();
        TAKE_SCREENSHOT = false;
    }
    glfwSwapBuffers(window->window);
}
void App::finish() {
    console.log("--finishing");
    gameState.reset();
    graphicEngine.reset();
    window.reset();
}

// todo: reset game loop on start..
// put in in some function that can be easliy called again when needed
void App::setGameState(std::shared_ptr<GameState> p_gameState) {
    FrameTime::timeOnStart = std::chrono::high_resolution_clock::now();
    FrameTime::lastTimePoint = FrameTime::timeOnStart;
    // cleanup after previous state?
    // cleanup resources, czy też zrobi się to automatycznie? automat będzie lepszy
    console.resetCounters();
    gameState = p_gameState;
}

void App::scrollCallback(GLFWwindow* w, double dx, double dy) {
    self->inputDispatcher->scrollCallback(dx, dy);
}
void App::keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    self->inputDispatcher->keyCallback(key, action, mods);
}
void App::mouseButtonCallback(GLFWwindow* w, int button, int action, int mods) {
    self->inputDispatcher->mouseButtonCallback(button, action, mods);
}
void App::cursorPosCallback(GLFWwindow* w, double xpos, double ypos) {
    // todo: distinguish on system and inGame pointer
    self->inputDispatcher->mousePosition(self->userPointer->screenPosition().x, self->userPointer->screenPosition().y);
    self->inputDispatcher->mouseMovement(self->userPointer->delta().x, self->userPointer->delta().y);
}
void App::exitCallback(GLFWwindow* w) {
    self->quit = true;
}
void App::errorCallback(int errorCode, const char* description) {
    console.error("[GLFW]", description);
}
