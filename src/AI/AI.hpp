#pragma once
#include "TaskCreator.hpp"
#include "TaskExecutor.hpp"
#include "Pathfinder.hpp"
#include "VehicleController.hpp"
#include "EventDispatcher.hpp"

class InputDispatcher;

class AiSharedState
{
private:
    InputDispatcher& m_inputDispatcher;
    EventDispatcher& m_eventDispatcher;
    std::unique_ptr<Pathfinder> m_pathfinder;
    std::unique_ptr<VehicleController> m_vehicleController;
    std::unique_ptr<TaksExecutor> m_taksExecutor;
    std::unique_ptr<TaskCreator> m_taskCreator;
public:
    AiSharedState(InputDispatcher& inputDispatcher) :
        m_pathfinder(std::make_unique<Pathfinder>()),
        m_vehicleController(std::make_unique<VehicleController>()),
        m_taksExecutor(std::make_unique<TaksExecutor>(*this)),
        m_taskCreator(std::make_unique<TaskCreator>(*this))
        {}

    InputDispatcher& inputDispatcher(){ return *InputDispatcher; }
    Pathfinder& pathfinder(){ return *m_pathfinder; }
    VehicleController& vehicleController(){ return *m_vehicleController; }
    TaksExecutor& taskExecutor(){ return *m_taskCreator; }
    TaskCreator& taskExecutor(){ return *m_taksExecutor; }
    EventDispatcher& eventDispatcher(){ return *m_eventDispatcher; }
};

struct SharedStateProvider
{
    AiSharedState& m_state;
    SharedStateProvider(AiSharedState& state) : m_state(state){}
};

class UserControlledAI : public Actor, public EventDispatcher, public AiSharedState
{
private:
public:
    UserControlledAI(InputDispatcher& inputDispatcher) :
        AiSharedState(InputDispatcher& inputDispatcher),
    {}

    void update(float dt){
        m_taksExecutor.updateTasks();
        m_vehicleController.moveVehicle();
    }
};
