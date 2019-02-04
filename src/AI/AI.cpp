#include "core.hpp"
#include "AI.hpp"
#include "AiControl.hpp"
#include "AiProcessor.hpp"
#include "AiProcessor.hpp"
#include "IPathfinder.hpp"
#include "NoPathfinder.hpp"
#include "Pathfinder.hpp"
#include "VehicleEquipment.hpp"


AI::AI(InputDispatcher& inputdispatcher, VehicleEquipment& vehicle, PointerInfo& pointerInfo, Scene& scene, Context& context):
        m_control(std::make_unique<AiControlViaInput>(inputdispatcher, pointerInfo)),
        m_vehicle(vehicle),
        m_pathfinder(std::make_unique<Pathfinder>(scene, context))
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
