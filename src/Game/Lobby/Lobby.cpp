#include "Lobby.hpp"
#include "ui.hpp"
#include "GraphicEngine.hpp"
#include "Context.hpp"
#include "ui-renderer.hpp"
#include "ui-text.hpp"
#include "Logging.hpp"
#include "LobbyEvents.hpp"

struct MainLobbyViewState : public LobbyViewState
{
    float toSlide {45.f};
    void render(Imgui& ui){
        // ui.panel().image("Logo").position(0.f, 1.f)();
        {
            auto& panel = ui.newFixedPanel()
                .width(350).height(1.f)
                .x(-450).y(0)
                .fill().color(0x6D3A31d0);
            panel.layout().toDown();
            panel.button().y(0.6f).w(0.9f).h(44)().formatting(Text::Centered).text("New Game");
            panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Continue");
            panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Settings");
            panel.slider().w(0.9f).h(44)(toSlide, 0.f, 100.f).formatting(Text::Centered).text("Volume " + toString(floor(toSlide)));
            panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Credits").action([]{
                log("Credits? Me! Lazy Falcon!");
            });
            panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Exit").action([]{
                event<ExitGame>();
            });
            panel();

        }

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
    renderer.context->setupFramebufferForGBufferGeneration();
    renderer.uiRender->render(m_ui.getToRender());
    renderer.context->endFrame();
}
