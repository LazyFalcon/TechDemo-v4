#pragma once
#include "AI.hpp"

class Input;
class InputDispatcher;
class TaksExecutor;
class EventDispatcher;

class ITaskCreator
{
public:
    virtual ~IOrderCreator() = default;
};

class TasksFromUserInput : public ITaskCreator, public SharedStateProvider
{
private:
    std::shared_ptr<Input> m_input;
public:
    TasksFromUserInput(AiSharedState& sharedState);
};
