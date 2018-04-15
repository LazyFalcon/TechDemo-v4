#include "App.hpp"
#include "Window.hpp"
#include "LobbyEvents.hpp"
#include "Lobby.hpp"

bool StartGame::handle(App &app){
    info("[ Start Lobby ]");

    auto lobby = std::make_shared<Lobby>();

    app.window->show();
    app.setGameState(lobby);
    return true;
}
