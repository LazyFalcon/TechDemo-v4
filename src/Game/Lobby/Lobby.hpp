#pragma once
#include "GameState.hpp"

class Imgui;
class Input;
class InputDispatcher;
class LobbyUI;
class Settings;

class Lobby : public GameState
{
private:
    std::shared_ptr<Input> m_input;
    Imgui& m_ui;
    std::unique_ptr<LobbyUI> m_view;
public:
    Lobby(Imgui&, InputDispatcher&, Settings&);
    ~Lobby();
    void update(float dt);
    void updateWithHighPrecision(float dt);
    void renderProcedure(GraphicEngine&);
};
