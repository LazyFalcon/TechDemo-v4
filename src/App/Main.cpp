#include <GLFW/glfw3.h>
#include "App.hpp"
#include "Audio/Audio.hpp"
#include "PerfTimers.hpp"
#include "LobbyEvents.hpp"

void initialEvent(){
    auto startGame = event<StartLobby>();
}

int main(int argc, char** argv){
    Audio::Device device;
    if(not device.init()) return -1;


    App app;
    if(not app.initialize()){
        error("Failed to initialize app");
        return -1;
    }

    app.setCommonCallbacks();

    // glfwSetScrollCallback(app.window->window, scrollCallback);
    // glfwSetKeyCallback(app.window->window, keyCallback);
    // glfwSetMouseButtonCallback(app.window->window, mouseButtonCallback);
    // glfwSetCursorPosCallback(app.window->window, cursorPosCallback);
    // glfwSetWindowCloseCallback(app.window->window, exitCallback);
    // glfwSetErrorCallback(errorCallback);

    CpuTimerScoped::printRecords = false;
    CpuTimerScoped::saveRecords = true;

    app.loadResources();
    // startBattleTestEvent(app);

    initialEvent();
    app.run();

    app.finish();

    return 0;
}
