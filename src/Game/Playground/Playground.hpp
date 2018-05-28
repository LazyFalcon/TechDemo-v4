#pragma once
#include "GameState.hpp"
#include "Scene.hpp"

class Imgui;
class Input;
class InputDispatcher;

class Playground : public GameState
{
private:
    Scene m_scene;
    std::shared_ptr<Input> m_input;
public:
    Playground(Imgui&, InputDispatcher&);
    ~Playground();
    void update(float dt);
    void updateWithHighPrecision(float dt);
    Scene& getScene();
    void renderProcedure(GraphicEngine&);
};
