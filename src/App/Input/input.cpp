#include "core.hpp"
#include <GLFW/glfw3.h>
#include "Logging.hpp"
#include "input.hpp"
#include "input-dispatcher.hpp"

Input::~Input(){
    deactivate();
    inputDispatcher.remove(this);
}

/**
 * At first check if there are binding with mode key
 * If not execute  bindings without mod keys
 */
bool Input::execute(int k, int a, int m, u32 currentTime){

    if(m_forwardAction) m_forwardAction(convertKeyToString(k,a,m));

    auto range = actions.equal_range(hashInput(k, a, m));
    if(range.second == range.first) range = actions.equal_range(hashInput(k, a, 0));


    bool releaseKey = false;

    for(auto it = range.first; it != range.second; ++it){
        u32 timeDifferenceFormLastAction = currentTime - it->second.lastTimeUsed;
        if(a == GLFW_REPEAT){
            if(it->second.lastTimeUsed == 0) it->second.lastTimeUsed = currentTime; // * case when we dont have .on set
            currentKey.onHoldTime = timeDifferenceFormLastAction;
        }
        else {
            it->second.noOfRepeats = (timeDifferenceFormLastAction < repeatTresholdTime) ? it->second.noOfRepeats+1 : 0;
            currentKey.noOfRepeats = it->second.noOfRepeats;
            it->second.lastTimeUsed = currentTime;
        }

        it->second();

        releaseKey |= currentKey.release;
        currentKey.release = false;
    }
    return releaseKey;
}
bool Input::executeTwoArgs(int arg, float x, float y){
    auto range = actions2f.equal_range(arg);
    for(auto it = range.first; it != range.second; ++it){
        it->second(x, y);
    }
    return false;
}

void Input::startHoldKey(int k, int m, u32 currentTime){
    auto range = actions.equal_range(hashInput(k, GLFW_REPEAT, m));
    if(range.second == range.first) range = actions.equal_range(hashInput(k, GLFW_REPEAT, 0));
    for(auto it = range.first; it != range.second; ++it){
        it->second.lastTimeUsed = currentTime;
    }
}
void Input::releaseHeldKey(int k, int m){
    auto range = actions.equal_range(hashInput(k, GLFW_REPEAT, m));
    if(range.second == range.first) range = actions.equal_range(hashInput(k, GLFW_REPEAT, 0));
    for(auto it = range.first; it != range.second; ++it){
        it->second.lastTimeUsed = 0;
    }
}
Input& Input::name(const std::string&){
        return *this;
    }
Input& Input::action(const std::string& binding){
    m_currentBinding = parseKeyBinding(binding);
    return *this;
}
Input& Input::defined(const std::string& funcName){
    m_currentBinding = parseKeyBinding(inputDispatcher.getDefined(funcName));
    return *this;
}
Input& Input::forward(std::function<void(const std::string&)>&& func){
    m_forwardAction = std::move(func);
    return *this;
}
Input& Input::on(Lambda&& func){
    m_currentBinding.action = GLFW_PRESS;
    actions.emplace(hashInput(m_currentBinding), Action(func));

    return *this;
}
Input& Input::hold(Lambda&& func){
    m_currentBinding.action = GLFW_REPEAT;
    actions.emplace(hashInput(m_currentBinding), Action(func));

    return *this;
}
Input& Input::off(Lambda&& func){
    m_currentBinding.action = GLFW_RELEASE;
    actions.emplace(hashInput(m_currentBinding), Action(func));

    return *this;
}
Input& Input::on(Lambda2Inputs&& func){
    actions2f.emplace(m_currentBinding.key, Action(func));

    return *this;
}

void Input::activate(){
    active = true;
}
void Input::deactivate(){
    active = false;
}
