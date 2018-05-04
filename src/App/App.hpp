#pragma once
#include "common.hpp"

class DebugScreen;
class EventProcessor;
class GameState;
class GLFWwindow;
class GraphicEngine;
class IAudio;
class Imgui;
class Input;
class InputDispatcher;
class ParticleProcessor;
class PhysicsWorld;
class Settings;
class Window;

class App
{
private:
    void initializeInputDispatcher();
    std::unique_ptr<DebugScreen> debugScreen;
    static App* self;
public:
    std::unique_ptr<Window> window;
    // std::unique_ptr<IAudio> audio;
    std::unique_ptr<GraphicEngine> graphicEngine;
    std::shared_ptr<GameState> gameState;
    std::unique_ptr<EventProcessor> eventProcessor;
    // std::unique_ptr<ParticleProcessor> particleProcessor;
    // std::unique_ptr<PhysicsWorld> physics;
    // std::unique_ptr<UI::Updater> uiUpdater;
    std::unique_ptr<Imgui> imgui;
    std::unique_ptr<InputDispatcher> inputDispatcher;
    std::shared_ptr<Input> input;
    std::unique_ptr<Settings> settings;

    bool quit {false};
    glm::vec2 lastCursorPos;
    App();
    ~App();

    bool initialize();
    void setCommonCallbacks();
    bool loadResources();
    void run();
    void exit(){
        quit = true;
    }
    void render();
    void finish();
    void setGameState(std::shared_ptr<GameState> gameState);
    void hideMouse();
    void showMouse();

    static void scrollCallback(GLFWwindow *w, double dx, double dy);
    static void keyCallback(GLFWwindow *w, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow *w, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* w, double xpos, double ypos);
    static void exitCallback(GLFWwindow *w);
    static void errorCallback(int errorCode, const char* description);
};
