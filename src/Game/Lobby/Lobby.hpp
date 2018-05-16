#pragma once
#include "GameState.hpp"
#include "Scene.hpp"

class Imgui;
class Input;
class InputDispatcher;
class LobbyUI;
class Settings;

class Lobby : public GameState
{
private:
    Scene m_scene;
    std::shared_ptr<Input> m_input;
    Imgui& m_ui;
    std::unique_ptr<LobbyUI> m_view;
public:
    Lobby(Imgui&, InputDispatcher&, Settings&);
    ~Lobby();
    void update(float dt);
    void updateWithHighPrecision(float dt);
    Scene& getScene();
    void renderProcedure(GraphicEngine&);
};
