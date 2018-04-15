#pragma once
#include "GameState.hpp"
#include "Scene.hpp"

class Lobby : public GameState
{
private:
    Scene m_scene;
public:
     void update(float dt);
     void updateWithHighPrecision(float dt);
     Scene& getScene();
     void renderSelf(GraphicEngine&);
};
