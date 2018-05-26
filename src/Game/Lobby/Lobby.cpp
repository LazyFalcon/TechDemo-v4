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
    Settings m_settingsBackup;
    enum Views {Misc, Video, Audio, Controls};
    int m_currentPanel = Misc;
    bool m_firstRun {true};

    void drawMisc(Imgui& ui, Panel& parentPanel){
        Panel panel(parentPanel);
        panel.color(0)();
        panel.layout().padding({20,20,20,20}).spacing(25).toDown(LEFT);
        panel.item().w(250).h(35)().text("Nothing to se here..");
    }

    DropDownList<glm::vec2> m_windowSizes;
    void drawVideo(Imgui& ui, Panel& parentPanel){
        Panel panel(parentPanel);
        panel.color(0)();
        panel.layout().padding({20,20,20,20}).spacing(25).toDown(LEFT);

        // ? panel.checkbox().w(160).h(35)(m_settings.fullscreen);
        checkbox(m_settings.video.fullscreen, "Fullscreen", panel, {250, 35});
        m_windowSizes.execute(panel, {250, 35});

    }
    void drawAudio(Imgui& ui, Panel& parentPanel){
        Panel panel(parentPanel);
        panel.color(0)();
        panel.layout().padding({20,20,20,20}).spacing(25).toDown(LEFT);
        panel.slider().w(0.9f).h(20)(m_settings.audio.masterVolume, 0.f, 100.f);
        panel.slider().w(0.9f).h(20)(m_settings.audio.musicVolume, 0.f, 100.f);
        panel.slider().w(0.9f).h(20)(m_settings.audio.effectsVolume, 0.f, 100.f);
        panel.slider().w(0.9f).h(20)(m_settings.audio.voiceVolume, 0.f, 100.f);

    }
    void drawControls(Imgui& ui, Panel& parentPanel){

    }

public:
    LobbySettings(Settings& settings) : m_settings(settings), m_windowSizes({{{"1600x900"}, {1600,900}}, {{"1920x1080"}, {1920,1080}}, {{"1920x1200"},{1920,1200}}}, {{"1600x900"}, {1600,900}}){}
    bool execute(Imgui& ui) override {
        if(m_firstRun) m_settingsBackup = m_settings;

        Panel panel(ui);
        panel.width(0.8f).height(0.8f)
            .x(0.1f).y(0.1f)
            .blured(0x10101010)();
        panel.layout().padding({20,20,20,20}).spacing(25).toDown(notEven({50, 1.f}));
        if(panel.onKey("on-esc")){
            return false;
        }

            Panel header(panel);
            header.color(0)();
            header.layout().padding({}).spacing(0).toRight(even(4));
            header.quickStyler([this](Item& item){
                item.formatting(Text::Centered).color(0);
                if(item.id()==m_currentPanel) item.font("ui_20_bold");
                item().action([this, &item]{m_currentPanel = item.id();});
            });
            header.button().text("Misc.");
            header.button().text("Video");
            header.button().text("Audio");
            header.button().text("Controls");

        switch(m_currentPanel){
            case Misc: drawMisc(ui, panel); break;
            case Video: drawVideo(ui, panel); break;
            case Audio: drawAudio(ui, panel); break;
            case Controls: drawControls(ui, panel); break;
            default:
                log("unknown panel", m_currentPanel);
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
            .blured(0x10101010)();
        panel.layout().spacing(15).toDown();
        panel.quickStyler([](Item& item){
            item.w(0.9f).h(44).formatting(Text::Centered);
        });
        // panel.emptySpace(0.6f);
        panel.button().y(0.6f)().text("New Game");
        panel.button()().text("Continue");
        panel.button()().text("Settings").action([this]{
            fadeOut([this]{fadeOut([this]{ m_currentState = m_settings; });});
        });
        panel.button()().text("Credits").action([this]{
            fadeOut([this]{log("Credits? Me! Lazy Falcon!");fadeIn();});
        });
        panel.button()().text("Exit").action([this]{
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
                    if(m_currentState->execute(ui)) return;
                    else {
                        m_currentState.reset();
                        fadeIn();
                    }
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
