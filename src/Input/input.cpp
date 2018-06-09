#include "core.hpp"
#include <GLFW/glfw3.h>
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
bool Input::execute(int k, int a, int m){

    if(m_forwardAction) m_forwardAction(convertKeyToString(k,a,m));

    auto range = actions.equal_range(hashInput(k, a, m));
    if(range.second == range.first) range = actions.equal_range(hashInput(k, a, 0));
    bool anyExecuted = false;
    for(auto it = range.first; it != range.second; ++it){
        it->second();
        anyExecuted = true;
    }
    return anyExecuted;
}
bool Input::executeTwoArgs(int arg, float x, float y){
    auto range = actions2f.equal_range(arg);
    bool anyExecuted = false;
    for(auto it = range.first; it != range.second; ++it){
        it->second(x, y);
        anyExecuted = true;
    }
    return anyExecuted;
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
    actions.emplace(hashInput(m_currentBinding), std::move(func));

    return *this;
}
Input& Input::hold(Lambda&& func){
    m_currentBinding.action = GLFW_REPEAT;
    actions.emplace(hashInput(m_currentBinding), std::move(func));

    return *this;
}
Input& Input::off(Lambda&& func){
    m_currentBinding.action = GLFW_RELEASE;
    actions.emplace(hashInput(m_currentBinding), std::move(func));

    return *this;
}
Input& Input::on(Lambda2F&& func){
    actions2f.emplace(m_currentBinding.key, std::move(func));

    return *this;
}

void Input::activate(){
    active = true;
}
void Input::deactivate(){
    active = false;
}
