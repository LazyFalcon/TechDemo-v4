#include "Context.hpp"
#include "GraphicEngine.hpp"
#include "input-dispatcher.hpp"
#include "input.hpp"
#include "Lobby.hpp"
#include "LobbyEvents.hpp"
#include "Logging.hpp"
#include "RendererUtils.hpp"
#include "Settings.hpp"
#include "Texture.hpp"
#include "ui-renderer.hpp"
#include "ui-text.hpp"
#include "ui.hpp"

struct LobbyViewState
{
    ~LobbyViewState() = default;
    virtual bool execute(Imgui& ui) = 0; // false means that state is finished
};

struct LobbySettings : public LobbyViewState
{
private:
    Settings& m_settings;
public:
    LobbySettings(Settings& settings) : m_settings(settings){}
    bool execute(Imgui& ui) override {


        auto& panel = ui.newFixedPanel()
            .width(0.8f).height(0.8f)
            .x(0.1f).y(0.1f)
            .blured(0x6D3A3150);

            auto& header = panel.newFixedPanel();
            header.width(0.95f).height(50)
                  .color(0x60606090);
            header();




        panel();

        return true;
    }
};

struct LobbyUI
{
private:
    std::function<void(void)> m_callAfterTransition;
    enum State {
        FadeIn, Main, FadeOut, External
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
            .blured(0x6D3A3150);
        panel.layout().toDown();
        panel.button().y(0.6f).w(0.9f).h(44)().formatting(Text::Centered).text("New Game");
        panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Continue");
        panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Settings").action([this]{
            fadeOut([this]{fadeOut([this]{ m_currentState = m_settings; });});
        });
        panel.slider().w(0.9f).h(44)(toSlide, 0.f, 100.f).formatting(Text::Centered).text("Volume " + toString(floor(toSlide)));
        panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Credits").action([this]{
            fadeOut([this]{log("Credits? Me! Lazy Falcon!");fadeIn();});
        });
        panel.button().w(0.9f).h(44)().formatting(Text::Centered).text("Exit").action([this]{
            fadeOut([]{event<ExitGame>();});
        });
        panel();
    }

    std::shared_ptr<LobbyViewState> m_currentState;
    std::shared_ptr<LobbyViewState> m_settings;

    float toSlide {45.f};
    float m_timer;
    float m_uiPos;
public:
    LobbyUI(Settings& settings) : m_settings(std::make_shared<LobbySettings>(settings)){
        fadeIn();
    }
    void run(Imgui& ui, float dt){
        switch(m_state){
            case FadeIn :
                m_timer -= dt;
                if(m_timer < 0.f) m_state = Main;

                m_uiPos = -1 - 450 * glm::smoothstep(0.f, 1.f, 1-m_timer / fadeTime);

                break;
            case Main :
                break;
            case FadeOut :
                m_timer -= dt;
                if(m_timer < 0.f){
                    m_state = External;
                    m_callAfterTransition();
                    return;
                }

                m_uiPos = -450 + 449 * glm::smoothstep(0.f, 1.f, 1-m_timer / fadeTime);

                break;
            case External:
                if(m_currentState){
                    if( m_currentState->execute(ui) == false){
                        m_currentState.reset();
                        fadeIn();
                    }
                    else return;
                }
                return;
        }
        render(ui, m_uiPos);
    }

};

Lobby::Lobby(Imgui& ui, InputDispatcher& inputDispatcher, Settings& settings): m_ui(ui), m_input(inputDispatcher.createNew("Lobby")){
    m_view = std::make_unique<LobbyUI>(settings);
    m_input->activate();
}
Lobby::~Lobby() = default;
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

    renderer.context->tex.gbuffer.color.genMipmaps();

    renderer.uiRender->render(m_ui.getToRender());
    renderer.context->endFrame();
}
