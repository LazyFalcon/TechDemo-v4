#pragma once
#include "common.hpp"

class GraphicEngine;
class Scene;

class GameState
{
public:
    virtual ~GameState() = default;
    virtual void update(float dt) = 0;
    virtual void updateWithHighPrecision(float dt) = 0;
    virtual Scene& getScene() = 0;
    virtual void renderProcedure(GraphicEngine&) = 0;
};
