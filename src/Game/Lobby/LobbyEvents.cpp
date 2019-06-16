#include "core.hpp"
#include "App.hpp"
#include "Lobby.hpp"
#include "LobbyEvents.hpp"
#include "Logger.hpp"
#include "Window.hpp"

bool StartLobby::handle(App &app){
    console.info("[ Start Lobby ]");

    auto lobby = std::make_shared<Lobby>(*app.imgui, *app.inputDispatcher, *app.settings);

    app.window->show();
    app.showMouse();
    app.setGameState(lobby);
    return true;
}

bool ExitGame::handle(App &app){
    console.info("[ Bye! ]");
    app.exit();
    return true;
}
