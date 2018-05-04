#pragma once
#include "GameState.hpp"
#include "Scene.hpp"

class Imgui;
class Input;
class InputDispatcher;

struct LobbyViewState
{
    ~LobbyViewState() = default;
    virtual void run(Imgui&, float) = 0;
};

class Lobby : public GameState
{
private:
    Scene m_scene;
    std::shared_ptr<Input> m_input;
    Imgui& m_ui;
    std::unique_ptr<LobbyViewState> m_view;
public:
    Lobby(Imgui&, InputDispatcher&);
    void update(float dt);
    void updateWithHighPrecision(float dt);
    Scene& getScene();
    void renderProcedure(GraphicEngine&);
};
