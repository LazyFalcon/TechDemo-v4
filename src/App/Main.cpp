#include "core.hpp"
#include <GLFW/glfw3.h>
#include "App.hpp"
#include "Audio/Audio.hpp"
#include "DefaultResourcePaths.hpp"
#include "LobbyEvents.hpp"
#include "PerfTimers.hpp"
#include "Logging.hpp"

void initialEvent(){
    auto startGame = event<StartLobby>();
}

int main(int argc, char** argv){
    resolvePaths(argv[0]);

    Audio::Device device;
    if(not device.init()) return -1;


    App app;
    if(not app.initialize()){
        error("Failed to initialize app");
        return -1;
    }

    app.setCommonCallbacks();

    CpuTimerScoped::printRecords = false;
    CpuTimerScoped::saveRecords = true;

    app.loadResources();

    initialEvent();
    app.run();

    app.finish();

    return 0;
}
