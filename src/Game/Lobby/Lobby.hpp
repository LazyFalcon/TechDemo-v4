#pragma once
#include "GameState.hpp"
#include "Scene.hpp"
#include "input-dispatcher.hpp"
#include "input.hpp"

class Imgui;

struct LobbyViewState
{
    ~LobbyViewState() = default;
    virtual void run(Imgui&, float) = 0;
};

class Lobby : public GameState
{
private:
    Scene m_scene;
    InputContextPtr m_input;
    Imgui& m_ui;
    std::unique_ptr<LobbyViewState> m_view;
public:
    Lobby(Imgui& ui, Input& parentInput);
    void update(float dt);
    void updateWithHighPrecision(float dt);
    Scene& getScene();
    void renderProcedure(GraphicEngine&);
};
