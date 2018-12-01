#include "core.hpp"
#include "App.hpp"
#include "Playground.hpp"
#include "LobbyEvents.hpp"
#include "Logging.hpp"
#include "PlaygroundEvents.hpp"
#include "Scene.hpp"
#include "Window.hpp"

bool StartPlayground::handle(App &app){
    info("[ Start Playground ]");

    auto playground = std::make_shared<Playground>(*app.imgui, *app.inputDispatcher, *app.window);

    auto& scene = playground->loadScene(sceneName);
    // pathifinderProcessAndSaveDepthMap(scene, context);
    // TODO: extract starting point
    try {
        playground->spawnPlayer("Drone", scene.spawnPoints.at(0).transform);
    } catch(std::out_of_range except){
        error("Noob! Scene have to has SpawnPoints defined!");
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
