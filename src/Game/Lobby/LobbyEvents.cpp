#include "App.hpp"
#include "Window.hpp"
#include "LobbyEvents.hpp"
#include "Lobby.hpp"

bool StartGame::handle(App &app){
    info("[ Start Lobby ]");

    auto lobby = std::make_shared<Lobby>(*app.imgui, app.inputContext);

    app.window->show();
    app.showMouse();
    app.setGameState(lobby);
    return true;
}

bool ExitGame::handle(App &app){
    info("[ Bye! ]");
    app.exit();
    return true;
}
