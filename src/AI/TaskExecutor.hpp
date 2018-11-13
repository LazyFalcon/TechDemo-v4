#pragma once
#include "AI.hpp"

namespace Tasks
{

struct Base
{
    virtual ~Base() = default;
    void precondition() = 0;
    bool update() = 0;
    void postcondition(){};
};

struct Moving : Base
{
    // * pointer to vehicleController
    // * pointer to generated path
    void precondition(){
        // * find and save path
    }
    bool update(){}
};


}


class TaskExecutor
{
private:
    std::list<std::unique_ptr<Tasks::Base>> m_tasks;
public:
    void moveTo(glm::vec4 target){
        auto& task = m_tasks.emplace_back(std::make_unique<Tasks::Moving>());
        taks->precondition();
    }
    void updateTasks(){
        m_tasks.remove_if([](auto& task){
            if(not task.update()){
                task.postcondition();
                return true;
            }
            return false;
        });
    }

};
