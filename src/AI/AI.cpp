#include "core.hpp"
#include "AI.hpp"
#include "AiControl.hpp"
#include "AiProcessor.hpp"
#include "AiProcessor.hpp"
#include "IPathfinder.hpp"
#include "NoPathfinder.hpp"
#include "VehicleEquipment.hpp"

AI::AI(InputDispatcher& inputdispatcher, VehicleEquipment& vehicle):
        m_control(std::make_unique<AiControlViaInput>(inputdispatcher)),
        m_vehicle(vehicle),
        m_pathfinder(std::make_unique<NoPathfinder>())
{
    m_processors.push_back(std::make_shared<VehicleControlProcessor>(*m_pathfinder, m_vehicle));

    m_control->newCommandCallback([this](AiCommand& command){
        for(auto& it : m_processors) it->newCommand(command);
    });
}

void AI::update(float dt){
    for(auto& it : m_processors) it->update(dt);
    m_vehicle.updateModules(dt);
}

void AI::updateGraphic(float dt){
    m_vehicle.updateMarkers();
    m_vehicle.drawBBOXesOfChildren();
    m_vehicle.graphics.toBeRendered();
}
