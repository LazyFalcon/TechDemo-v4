#pragma once
#include "core.hpp"
#include "AiCommand.hpp"
#include "IPathfinder.hpp"
#include "VehicleEquipment.hpp"
#include "Logging.hpp"

class AiProcessor
{
public:
    virtual void newCommand(AiCommand& command) = 0;
    virtual void update(float dt) = 0;
};

class VehicleControlProcessor : public AiProcessor
{
public:
    VehicleControlProcessor(IPathfinder& pathfinder, VehicleEquipment& vehicle) :
        m_pathfinder(pathfinder),
        m_vehicle(vehicle)
    {}

    void newCommand(AiCommand& command) override {
        auto cmd = command.get<MoveCommand>();
        if(not command.queue) m_queue.clear();
        m_queue.push_back({cmd.position, cmd.direction ? *cmd.direction : glm::vec4(), 20.f});
        if(not command.queue) calculateNewPath();
    }
    void update(float dt) override { // ? in ms?
        if(m_path.empty()){ // * current path finished
            if(m_queue.empty()){ // * queue of targets empty
                m_vehicle.control.targetPoint.reset();
                return;
            }
            else calculateNewPath();
        }


        const auto & waypoint = m_path[m_waypointID];

        float distanceInNextFrame = waypoint.velocity * dt/1000.f;
        auto position = m_vehicle.getPosition();
        auto vec = waypoint.position - position;
        float distance = glm::length(vec);


        auto next = position + vec/distance * distanceInNextFrame;

        if(distanceInNextFrame < distance){
            next = waypoint.position;
            if(m_waypointID <= m_path.size()) m_path.clear();
            else m_waypointID++;
        }

        m_vehicle.control.targetPoint = next;
    }
private:
    IPathfinder& m_pathfinder;
    VehicleEquipment& m_vehicle;
    std::list<Waypoint> m_queue;
    std::vector<Waypoint> m_path;
    int m_waypointID {0};
    glm::vec4 m_direction;

    void calculateNewPath(){
        if(m_queue.empty()) return;
        m_path = m_pathfinder.calculate({m_vehicle.getPosition(), glm::vec4(1,0,0,0), 20.f}, m_queue.front());
        m_waypointID = 0;
    }

};
