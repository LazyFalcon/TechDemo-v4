#include "core.hpp"
#include "AI.hpp"
#include "AiControl.hpp"
#include "AiProcessor.hpp"
#include "AiProcessor.hpp"
#include "IPathfinder.hpp"
#include "NoPathfinder.hpp"
#include "Pathfinder.hpp"
#include "VehicleEquipment.hpp"

AI::AI(std::unique_ptr<AiControl> control, std::unique_ptr<IPathfinder> pathfinder, VehicleEquipment& vehicle):
        m_control(std::move(control)),
        m_pathfinder(std::move(pathfinder)),
        m_vehicle(vehicle)
{
    m_processors.push_back(std::make_shared<VehicleControlProcessor>(*m_pathfinder, m_vehicle));

    m_control->newCommandCallback([this](AiCommand& command){
        for(auto& it : m_processors) it->newCommand(command);
    });

    m_pathfinder->preprocessMap();
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
