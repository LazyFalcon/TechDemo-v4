#include "Lobby.hpp"
#include "IMGUI.hpp"

struct MainLobbyViewState : public LobbyView
{
    void render(UI::IMGUI& ui){

    }
};



Lobby::Lobby(UI::IMGUI& ui, InputContextHandler &parentInput): m_ui(ui),  m_input(parentInput.derive("Lobby")), lobbyUi(m_ui){
    m_view = std::make_unique<MainLobbyViewState>();
    m_input->activate();
}

void Lobby::update(float dt){
    m_lobyUi.update();
}
void Lobby::updateWithHighPrecision(float dt){}
Scene&Lobby:: getScene(){
    return m_scene;
}
void Lobby::renderSelf(GraphicEngine&){}
