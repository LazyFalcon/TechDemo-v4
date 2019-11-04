#pragma once
#include "GameState.hpp"

class Imgui;
class Input;
class InputDispatcher;
class InputUserPointer;
class LobbyUI;
class Settings;

class Lobby : public GameState
{
private:
    Imgui& m_ui;
    InputUserPointer& m_inputUserPointer;
    std::shared_ptr<Input> m_input;
    std::unique_ptr<LobbyUI> m_view;
public:
    Lobby(Imgui&, InputDispatcher&, Settings&, InputUserPointer&);
    ~Lobby();
    void update(float dt);
    void updateWithHighPrecision(float dt);
    void renderProcedure(GraphicEngine&);
};
