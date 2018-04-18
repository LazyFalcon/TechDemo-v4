#include "Lobby.hpp"
#include "Imgui.hpp"

struct MainLobbyViewState : public LobbyViewState
{
    void render(Imgui& ui){
        // ui.panel().image("Logo").position(0.f, 1.f)();
        auto& panel = ui.newFixedPanel().width(250).height(1.f);
            panel();
    }
};



Lobby::Lobby(Imgui& ui, InputContextHandler &parentInput): m_ui(ui), m_input(parentInput.derive("Lobby")){
    m_view = std::make_unique<MainLobbyViewState>();
    m_input->activate();
}

void Lobby::update(float dt){
    // m_ui.update();
    m_view->render(m_ui);
}
void Lobby::updateWithHighPrecision(float dt){}
Scene&Lobby:: getScene(){
    return m_scene;
}
void Lobby::renderSelf(GraphicEngine&){}
