#include <GLFW/glfw3.h>
#include "input-dispatcher.hpp"
#include "input-utils.hpp"
#include "input.hpp"
#include "Logging.hpp"


std::shared_ptr<Input> InputDispatcher::createNew(const std::string& name){
    m_activeIputHandlers.emplace_back(std::make_shared<Input>(*this, name));
    return m_activeIputHandlers.back();
}

// passes input to active context, and recursive to its parents
void InputDispatcher::execute(int k, int a, int m){
    if(a != GLFW_PRESS and a != GLFW_RELEASE) return; /// GLFW_REPEAT is handled internally in funcion refresh as default behaviour for repeating keys sucks

    if(a == GLFW_PRESS){
        currentlyPressedKeys.push_back(k);
    }
    else if(a == GLFW_RELEASE){
        currentlyPressedKeys.remove(k);
    }

    currentModifierKey = m;
    for(auto& i : m_activeIputHandlers)
        if(i->active) i->execute(k, a, m);
}

void InputDispatcher::setPredefiniedBinding(const std::string& combination){
    auto funcAndKeys = splitToFunctionAndKeys(combination);
    configuredActions.emplace(funcAndKeys.first, funcAndKeys.second);
}

void InputDispatcher::refresh(){
    for(auto &it : currentlyPressedKeys){
        for(auto& i : m_activeIputHandlers)
            if(i->active) i->execute(it, GLFW_REPEAT, currentModifierKey);
    }
}
void InputDispatcher::scrollCallback(double dx, double dy){
    if(dy > 0) execute(SCROLL_UP, GLFW_PRESS, currentModifierKey);
    if(dy < 0) execute(SCROLL_DOWN, GLFW_PRESS, currentModifierKey);
}
void InputDispatcher::keyCallback(int key, int action, int mods){
    execute(key, action, mods);
}
void InputDispatcher::mouseButtonCallback(int button, int action, int mods){
    switch(button){
        case GLFW_MOUSE_BUTTON_LEFT: { button = LMB; break; }
        case GLFW_MOUSE_BUTTON_RIGHT: { button = RMB; break; }
        case GLFW_MOUSE_BUTTON_MIDDLE: { button = MMB; break; }
    }
    execute(button, action, mods);
}
void InputDispatcher::mousePosition(float x, float y){
    for(auto& i : m_activeIputHandlers)
        if(i->active) i->executeTwoArgs(MousePosition, x, y);
}
void InputDispatcher::mouseMovement(float x, float y){
    for(auto& i : m_activeIputHandlers)
        if(i->active) i->executeTwoArgs(MouseMove, x, y);
}
void InputDispatcher::joyPadDispatch(){}
