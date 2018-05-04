#include <GLFW/glfw3.h>
#include "Input.hpp"
#include "input-dispatcher.hpp"
#include "input-utils.hpp"
/**
 * At first check if there are binding with mode key
 * If not execute  bindings without mod keys
 */
bool Input::execute(int k, int a, int m){
    auto range = actions.equal_range(hashInput(k, a, m));
    if(range.second == range.first) range = actions.equal_range(hashInput(k, a, 0));
    bool anyExecuted = false;
    for(auto it = range.first; it != range.second; ++it){
        it->second();
        anyExecuted = true;
    }
    if(parent) anyExecuted = parent->execute(k, a, m);
    return anyExecuted;
}
bool Input::executeTwoArgs(int arg, float x, float y){
    auto range = actions2f.equal_range(arg);
    bool anyExecuted = false;
    for(auto it = range.first; it != range.second; ++it){
        it->second(x, y);
        anyExecuted = true;
    }
    if(parent) parent->executeTwoArgs(arg, x, y);
    return anyExecuted;
}

void Input::setAction(const std::string &actionName, Lambda onEnter, Lambda onExit){
    auto keys = inputDispatcher.getBinding(actionName);
    for(auto it = keys.first; it != keys.second; ++it){
        setAction(actionName, it->second, onEnter, onExit);
    }
}
void Input::setAction(const std::string &binding, const std::string &name, Lambda onEnter, Lambda onExit){
    auto keys = parseKeyBinding(binding);
    if(onEnter){
        auto hashed = hashInput(keys.key, keys.action, keys.modifier);
        actions.emplace(hashed, Action{name, onEnter});
    }
    keys.action = GLFW_RELEASE;
    if(onExit){
        auto hashed = hashInput(keys.key, keys.action, keys.modifier);
        actions.emplace(hashed, Action{name, onExit});
    }
}
void Input::setAction(const std::string &binding, const std::string &name, Lambda2F function){
    auto keys = parseKeyBinding(binding);
    // auto hashed = hashInput(keys.key, keys.action, keys.modifier);
    actions2f.emplace(keys.key, Action2F{name, function});
}
void Input::activate(){
    lastActive = inputDispatcher.active;
    inputDispatcher.active = this;
}
// TODO: add check if last active exists
// TODO: also deactivate children
void Input::deactivate(){
    if(inputDispatcher.active == this){
        inputDispatcher.active = lastActive;
        // inputDispatcher.active -> activate(); ??
    }
}
std::shared_ptr<Input> Input::derive(const std::string& newName){
    auto child = std::make_shared<Input>(inputDispatcher, newName);

    child->parent = this;
    children.push_back(child);

    return child;
}
