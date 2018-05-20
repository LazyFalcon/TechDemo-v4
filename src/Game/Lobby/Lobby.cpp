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
#include "ui-widgets.hpp"

struct LobbyViewState
{
    ~LobbyViewState() = default;
    virtual bool execute(Imgui& ui) = 0; // false means that state is finished
};

struct LobbySettings : public LobbyViewState
{
private:
    Settings& m_settings;
    enum Views {Misc, Video, Audio, Controls};
    Views m_currentPanel = Misc;

    void drawMisc(Imgui& ui, Panel& parentPanel){
        Panel panel(parentPanel);
        panel.color(0x01010110)();
        panel.layout().toDown();

        panel.item().w(250).h(35)().text("Nothing to se here..");
    }

    DropDownList<glm::vec2> m_windowSizes;
    void drawVideo(Imgui& ui, Panel& parentPanel){
        Panel panel(parentPanel);
        panel.color(0x01010110)();
        panel.layout().toDown();

        // panel.checkbox().w(160).h(35)(m_settings.fullscreen);
        m_windowSizes.execute(panel, {250, 35});

    }
    void drawAudio(Imgui& ui, Panel& parentPanel){

    }
    void drawControls(Imgui& ui, Panel& parentPanel){

    }

public:
    // LobbySettings(Settings& settings) : m_settings(settings), m_windowSizes({{{"1600x900"}, {1600,900}}}, {{"1600x900"}, {1600,900}}){}
    LobbySettings(Settings& settings) : m_settings(settings), m_windowSizes({{{"1600x900"}, {1600,900}}, {{"1920x1080"}, {1920,1080}}, {{"1920x1200"},{1920,1200}}}, {{"1600x900"}, {1600,900}}){}
    bool execute(Imgui& ui) override {


        Panel panel(ui);
        panel.width(0.8f).height(0.8f)
            .x(0.1f).y(0.1f)
            .blured(0x6D3A3150)();
        panel.layout().padding({5,20,5,20}).toDown(notEven({50, 1.f}));
        {
            Panel header(panel);
            header.color(0x60606090)();
            header.layout().padding({}).spacing(0).toRight(even(4));
            header.button().color(0xf0f0f090)().formatting(Text::Centered).text("Misc.").action([this]{m_currentPanel = Misc;});
            header.button().color(0x00000090)().formatting(Text::Centered).text("Video").action([this]{m_currentPanel = Video;});
            header.button().color(0xf0f0f090)().formatting(Text::Centered).text("Audio").action([this]{m_currentPanel = Audio;});
            header.button().color(0x00000090)().formatting(Text::Centered).text("Controls").action([this]{m_currentPanel = Controls;});
        }

        switch(m_currentPanel){
            case Misc: drawMisc(ui, panel); break;
            case Video: drawVideo(ui, panel); break;
            case Audio: drawAudio(ui, panel); break;
            case Controls: drawControls(ui, panel); break;
        }


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
        Panel panel(ui);
        panel.width(350).height(1.f)
            .x(startPosition).y(0)
            .blured(0x6D3A3150)();
        panel.layout().toDown();
        // panel.emptySpace(0.6f);
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
