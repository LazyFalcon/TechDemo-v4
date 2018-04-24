#include "Lobby.hpp"
#include "Imgui.hpp"
#include "GraphicEngine.hpp"
#include "Context.hpp"
#include "UIRender.hpp"

struct MainLobbyViewState : public LobbyViewState
{
    void render(Imgui& ui){
        // ui.panel().image("Logo").position(0.f, 1.f)();
        auto& panel = ui.newFixedPanel();
            panel.width(250);
            panel.height(1.f);
            panel.x(500);
            panel.y(0);
            // panel.fill().color(0x0d0d0dff);
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
void Lobby::renderProcedure(GraphicEngine& renderer){
    renderer.context->beginFrame();
    // renderer.uiRender->render(m_ui.getToRender());
    renderer.context->endFrame();
}
