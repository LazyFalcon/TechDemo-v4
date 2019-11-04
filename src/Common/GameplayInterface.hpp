#pragma once
#include "Utils.hpp"
#include "ui.hpp"
#include "GameSettings.hpp"

namespace camera{ class Camera; }
class Scene;
class GraphicEngine;

class GameplayInterface
{
public:
    virtual ~GameplayInterface(){}
    virtual void update(float dt) = 0;
    virtual void updateHighPrecision(float dt) = 0;
    virtual Scene& getScene() = 0;
    virtual void render(GraphicEngine&) = 0;
};
