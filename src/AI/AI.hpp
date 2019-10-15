#pragma once
#include "Actor.hpp"

class AiProcessor;
class AiControl;
class Context;
class InputDispatcher;
class IPathfinder;
class Scene;
class Vehicle;

struct AiSharedState
{
    bool isMoving;
};

class AI : public Actor
{
public:
    AI(std::unique_ptr<AiControl> control, std::unique_ptr<IPathfinder> pathfinder, Vehicle& vehicle);

    void update(float dt) override;
    void updateGraphic(float dt) override;
    void focusOn() override {}
    void focusOff() override {}
    bool hasFocus() override {return false;}
    Vehicle& eq() override {
        return m_vehicle;
    }
private:
    AiSharedState m_state;
    std::unique_ptr<AiControl> m_control;
    std::unique_ptr<IPathfinder> m_pathfinder;
    Vehicle& m_vehicle;
    std::vector<std::shared_ptr<AiProcessor>> m_processors;
};
