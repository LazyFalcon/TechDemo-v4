#include "core.hpp"
#include "App.hpp"
#include "Context.hpp"
#include "Context.hpp"
#include "GraphicEngine.hpp"
#include "LobbyEvents.hpp"
#include "Logging.hpp"
#include "Playground.hpp"
#include "PlaygroundEvents.hpp"
#include "Scene.hpp"
#include "Window.hpp"

bool StartPlayground::handle(App &app){
    info("[ Start Playground ]");

    auto playground = std::make_shared<Playground>(*app.imgui, *app.inputDispatcher, *app.window);

    auto& scene = playground->loadScene(sceneName);
    // pathifinderProcessAndSaveDepthMap(scene, context);
    // TODO: extract starting point
    if(scene.spawnPoints.size() == 0) error("Noob! Scene have to has SpawnPoints defined!");
    else {
        playground->spawnPlayer(vehicleName, scene.spawnPoints[0].transform);
        // if(scene.spawnPoints.size() > 1) for(auto i=1; i<scene.spawnPoints.size(); i++)
        //     playground->spawnHostileBot("Drone", scene.spawnPoints[i].transform);
    }

    app.window->show();
    app.showMouse();
    app.setGameState(playground);

    return true;
}

bool ExitPlayground::handle(App &app){
    event<StartLobby>();
    return true;
}
