#include "core.hpp"
#include "App.hpp"
#include "Playground.hpp"
#include "LobbyEvents.hpp"
#include "Logging.hpp"
#include "PlaygroundEvents.hpp"
#include "Window.hpp"

bool StartPlayground::handle(App &app){
    info("[ Start Playground ]");

    auto playground = std::make_shared<Playground>(*app.imgui, *app.inputDispatcher, *app.window);

    playground->spawnPlayer("Turret", {0,0,0,1});

    app.window->show();
    app.showMouse();
    app.setGameState(playground);


    return true;
}

bool ExitPlayground::handle(App &app){
    event<StartLobby>();
    return true;
}
