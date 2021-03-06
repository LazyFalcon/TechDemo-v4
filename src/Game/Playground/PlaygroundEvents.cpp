#include "core.hpp"
#include "PlaygroundEvents.hpp"
#include "App.hpp"
#include "Context.hpp"
#include "GraphicEngine.hpp"
#include "LobbyEvents.hpp"
#include "Logger.hpp"
#include "Playground.hpp"
#include "Scene.hpp"
#include "Window.hpp"
#include "input-user-pointer.hpp"

bool StartPlayground::handle(App& app) {
    console.info("[ Start Playground ]");

    auto playground = std::make_shared<Playground>(*app.imgui, *app.inputDispatcher, *app.window, *app.userPointer);

    auto& scene = playground->loadScene(sceneName);
    // pathifinderProcessAndSaveDepthMap(scene, context);
    // TODO: extract starting point
    if(scene.spawnPoints.size() == 0)
        console.error("Noob! Scene have to has SpawnPoints defined!");
    else {
        playground->spawnPlayer(vehicleName, scene.spawnPoints[0].transform);
        // if(scene.spawnPoints.size() > 1) for(auto i=1; i<scene.spawnPoints.size(); i++)
        //     playground->spawnHostileBot("Drone", scene.spawnPoints[i].transform);
    }

    app.window->show();
    app.userPointer->hideSystemCursor();
    app.setGameState(playground);

    console.log("*Game has been started*");
    return true;
}

bool ExitPlayground::handle(App& app) {
    event<StartLobby>();
    return true;
}
