#pragma once
#include "GameState.hpp"
#include "Scene.hpp"
#include "LobbyUI.hpp"
#include "InputHandler.hpp"
#include "InputContextHandler.hpp"

namespace UI{
    class IMGUI;
}

struct LobbyView
{
    ~LobbyView() = default;
    virtual void render(UI::IMGUI& ui) = 0;
};

class Lobby : public GameState
{
private:
    Scene m_scene;
    InputContextPtr m_input;
    UI::IMGUI& m_ui;
    std::unique_ptr<LobbyViewState> m_view;
public:
    Lobby(UI::IMGUI& ui, InputContextHandler& parentInput);
     void update(float dt);
     void updateWithHighPrecision(float dt);
     Scene& getScene();
     void renderSelf(GraphicEngine&);
};
