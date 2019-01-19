#pragma once
#include "Actor.hpp"
#include "AiControl.hpp"
#include "IPathfinder.hpp"

class AiProcessor;
class InputDispatcher;
class VehicleEquipment;

struct AiSharedState
{
    bool isMoving;
};

class AI : public Actor
{
public:
    AI(InputDispatcher& inputdispatcher, VehicleEquipment& vehicle);

    void update(float dt) override;
    void updateGraphic(float dt) override;
    void focusOn() override {}
    void focusOff() override {}
    VehicleEquipment& eq() override {
        return m_vehicle;
    }
private:
    AiSharedState m_state;
    std::unique_ptr<AiControl> m_control;
    VehicleEquipment& m_vehicle;
    std::unique_ptr<IPathfinder> m_pathfinder;
    std::vector<std::shared_ptr<AiProcessor>> m_processors;
};
