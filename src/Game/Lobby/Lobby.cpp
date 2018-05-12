#include "Lobby.hpp"
#include "Context.hpp"
#include "GraphicEngine.hpp"
#include "input-dispatcher.hpp"
#include "input.hpp"
#include "LobbyEvents.hpp"
#include "Logging.hpp"
#include "RendererUtils.hpp"
#include "ui-renderer.hpp"
#include "ui-text.hpp"
#include "ui.hpp"

struct MainLobbyViewState : public LobbyViewState
{
private:
    std::function<void(void)> m_callAfterTransition;
    enum State {
        FadeIn, Main, FadeOut
    };
    State m_state;
    const float fadeTime = 0.1f * 1000;

    void fadeIn(){
        m_state = FadeIn;
        m_timer = fadeTime;
    }
    void fadeOut(std::function<void(void)> f){
        m_state = FadeOut;
        m_timer = fadeTime;
        m_callAfterTransition = f;
    }

    void render(Imgui& ui, float startPosition){
        // ui.panel().image("Logo").position(0.f, 1.f)();
        auto& panel = ui.newFixedPanel()
            .width(350).height(1.f)
            .x(startPosition).y(0)
            // .x(-450).y(0)
            .fill().color(0x6D3A31d0);
        panel.layout().toDown();
        panel.button().y(0.6f).w(0.9f).h(44)().formatting(Text::Centered).text("New Game");
        panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Continue");
        panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Settings");
        panel.slider().w(0.9f).h(44)(toSlide, 0.f, 100.f).formatting(Text::Centered).text("Volume " + toString(floor(toSlide)));
        panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Credits").action([this]{
            fadeOut([this]{log("Credits? Me! Lazy Falcon!");fadeIn();});
        });
        panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Exit").action([this]{
            fadeOut([]{event<ExitGame>();});
        });
        panel();
    }

    float toSlide {45.f};
    float m_timer;
public:
    MainLobbyViewState(){
        fadeIn();
    }
    void run(Imgui& ui, float dt) override {
        float pos = -450.f;
        switch(m_state){
            case FadeIn : {
                m_timer -= dt;
                if(m_timer < 0.f) m_state = Main;

                pos = -1 - 450 * glm::smoothstep(0.f, 1.f, 1-m_timer / fadeTime);

                break;
            }
            case Main : {break;}
            case FadeOut : {
                m_timer -= dt;
                if(m_timer < 0.f){
                    m_state = Main;
                    m_callAfterTransition();
                }

                pos = -450 + 449 * glm::smoothstep(0.f, 1.f, 1-m_timer / fadeTime);

                break;
            }
        }
        render(ui, pos);
    }

};

Lobby::Lobby(Imgui& ui, InputDispatcher& inputDispatcher): m_ui(ui), m_input(inputDispatcher.createNew("Lobby")){
    m_view = std::make_unique<MainLobbyViewState>();
    m_input->activate();
}

void Lobby::update(float dt){
    // m_ui.update();
    m_view->run(m_ui, dt);

}
void Lobby::updateWithHighPrecision(float dt){}
Scene&Lobby:: getScene(){
    return m_scene;
}
void Lobby::renderProcedure(GraphicEngine& renderer){
    renderer.context->beginFrame();
    renderer.context->setupFramebufferForGBufferGeneration();
    renderer.utils->drawBackground("nebula");
    renderer.uiRender->render(m_ui.getToRender());
    renderer.context->endFrame();
}
