#pragma once
#include "GameState.hpp"
#include "Scene.hpp"
#include "InputHandler.hpp"
#include "InputContextHandler.hpp"

class Imgui;

struct LobbyViewState
{
    ~LobbyViewState() = default;
    virtual void render(Imgui& ui) = 0;
};

class Lobby : public GameState
{
private:
    Scene m_scene;
    InputContextPtr m_input;
    Imgui& m_ui;
    std::unique_ptr<LobbyViewState> m_view;
public:
    Lobby(Imgui& ui, InputContextHandler& parentInput);
    void update(float dt);
    void updateWithHighPrecision(float dt);
    Scene& getScene();
    void renderSelf(GraphicEngine&);
};
