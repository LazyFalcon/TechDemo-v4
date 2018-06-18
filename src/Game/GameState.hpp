#pragma once

class GraphicEngine;

class GameState
{
public:
    virtual ~GameState() = default;
    virtual void update(float dt) = 0;
    virtual void updateWithHighPrecision(float dt) = 0;
    virtual void renderProcedure(GraphicEngine&) = 0;
};
