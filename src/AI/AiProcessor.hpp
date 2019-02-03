#pragma once
#include "core.hpp"
#include "AiCommand.hpp"
#include "IPathfinder.hpp"
#include "VehicleEquipment.hpp"
#include "Logging.hpp"

class AiProcessor
{
public:
    virtual ~AiProcessor() = default;
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
        m_queue.push_back({cmd.position, cmd.direction ? *cmd.direction : glm::vec4(), 40.f});
        if(not command.queue) calculateNewPath();
    }
    auto selectNextWaypoint(float distanceToGo){
        float distance;
        glm::vec4 vec;

        // * select next waypoint until next waypoint is further than distance to go, or it is last point
        do {
            vec = m_waypointID->position - m_vehicle.getPosition();
            distance = glm::length(vec);
        } while(distance < distanceToGo and m_waypointID++ < m_lastWaypoint);

        return std::make_tuple(vec, distance);
    }
    void update(float dt) override { // ? in ms?
        if(m_path.empty()){ // * current path finished
            if(m_queue.empty()){ // * queue of targets empty
                m_vehicle.control.targetPoint.reset();
                return;
            }
            else calculateNewPath();
        }

        float distanceToGo = m_waypointID->velocity * dt/1000.f;

        auto [vec, distance] = selectNextWaypoint(distanceToGo);

        auto botPosition = m_vehicle.getPosition();
        auto next = botPosition + vec/distance * distanceToGo;

        if(distance >= distanceToGo and m_waypointID == m_lastWaypoint){
            next = m_waypointID->position;
            m_path.clear();
        }

        m_vehicle.control.targetPoint = next;
    }


private:
    IPathfinder& m_pathfinder;
    VehicleEquipment& m_vehicle;
    std::list<Waypoint> m_queue; // * list of orders
    std::vector<Waypoint> m_path; // * path generated by pathfinder
    std::vector<Waypoint> m_splinepath; // * B-Spline generated from path, more dense than path
    std::vector<Waypoint>::iterator m_waypointID; // * current point from path
    std::vector<Waypoint>::iterator m_lastWaypoint; // * current point from path
    glm::vec4 m_direction;

    void calculateNewPath(){
        if(m_queue.empty()) return;
        m_path = m_pathfinder.calculate({m_vehicle.getPosition(), glm::vec4(1,0,0,0), 40.f}, m_queue.front());
        m_waypointID = m_path.begin();
        m_lastWaypoint = m_path.end()-1;
        m_queue.pop_front();
    }

};
