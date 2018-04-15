#pragma once
#include "Includes.hpp"

class GraphicEngine;
class Scene;

class GameState
{
public:
    virtual ~GameState() = default;
    virtual void update(float dt) = 0;
    virtual void updateWithHighPrecision(float dt) = 0;
    virtual Scene& getScene() = 0;
    virtual void renderSelf(GraphicEngine&) = 0;
};
