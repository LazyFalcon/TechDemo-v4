#pragma once
#include "GameState.hpp"
#include "Scene.hpp"

class Imgui;
class Input;
class InputDispatcher;
class Player;

class Playground : public GameState
{
private:
    Scene m_scene;
    std::shared_ptr<Input> m_input;
    std::shared_ptr<Player> m_player;
public:
    Playground(Imgui&, InputDispatcher&);
    ~Playground();
    void update(float dt);
    void updateWithHighPrecision(float dt);
    Scene& getScene();
    void renderProcedure(GraphicEngine&);

    void spawnPlayer(const std::string&, glm::vec4);
};
