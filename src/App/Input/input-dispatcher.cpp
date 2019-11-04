#include "core.hpp"
#include "input-dispatcher.hpp"
#include <GLFW/glfw3.h>
#include "Logger.hpp"
#include "input-utils.hpp"
#include "input.hpp"

std::shared_ptr<Input> InputDispatcher::createNew(const std::string& name) {
    m_activeInputHandlers.emplace_back(std::make_shared<Input>(*this, name));
    return m_activeInputHandlers.back();
}
void InputDispatcher::remove(Input* toRemove) {
    m_activeInputHandlers.remove_if([toRemove](std::shared_ptr<Input>& it) { return it.get() == toRemove; });
}

// passes input to active context, and recursive to its parents
void InputDispatcher::execute(int k, int a, int m) {
    if(a != GLFW_PRESS and a != GLFW_RELEASE)
        return; /// GLFW_REPEAT is handled internally in function refresh as default behaviour for repeating keys sucks

    if(a == GLFW_PRESS) {
        currentlyPressedKeys.push_back(k);
        for(auto& i : m_activeInputHandlers)
            if(i->active)
                i->startHoldKey(k, m, m_msFromStart);
    }
    else if(a == GLFW_RELEASE) {
        currentlyPressedKeys.remove(k);
        for(auto& i : m_activeInputHandlers)
            if(i->active)
                i->releaseHeldKey(k, m);
    }

    m_currentModifierKey = m;
    for(auto& i : m_activeInputHandlers)
        if(i->active)
            i->execute(k, a, m, m_msFromStart);
}

InputDispatcher& InputDispatcher::defineBinding(const std::string& name, const std::string& keys) {
    m_configuredActions[name] = keys;
    return *this;
}
const std::string& InputDispatcher::getDefined(const std::string& name) {
    return m_configuredActions[name];
}

void InputDispatcher::heldUpKeys() {
    for(auto& key : currentlyPressedKeys)
        for(auto& handler : m_activeInputHandlers)
            if(handler->active
               and handler->execute(key, GLFW_REPEAT, m_currentModifierKey,
                                    m_msFromStart)) { // * then release this key from repeating
                currentlyPressedKeys.remove(key);
                for(auto& i : m_activeInputHandlers)
                    if(i->active)
                        i->releaseHeldKey(key, m_currentModifierKey);
            }
}
void InputDispatcher::scrollCallback(double dx, double dy) {
    if(dy > 0)
        execute(SCROLL_UP, GLFW_PRESS, m_currentModifierKey);
    if(dy < 0)
        execute(SCROLL_DOWN, GLFW_PRESS, m_currentModifierKey);
    m_mouseState.scrollDx = dx;
    m_mouseState.scrollDy = dy;
}
void InputDispatcher::keyCallback(int key, int action, int mods) {
    execute(key, action, mods);
}
void InputDispatcher::mouseButtonCallback(int button, int action, int mods) {
    switch(button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            button = LMB;
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT: {
            button = RMB;
            break;
        }
        case GLFW_MOUSE_BUTTON_MIDDLE: {
            button = MMB;
            break;
        }
    }
    execute(button, action, mods);
}
void InputDispatcher::mousePosition(float x, float y) {
    m_mouseState.pointerScreenPosition = {x, y};
    for(auto& i : m_activeInputHandlers)
        if(i->active)
            i->executeTwoArgs(MousePosition, x, y);
}
void InputDispatcher::mouseMovement(float x, float y) {
    m_mouseState.pointerRelativeScreenMove = {x, y};
    for(auto& i : m_activeInputHandlers)
        if(i->active)
            i->executeTwoArgs(MouseMove, x, y);
}
void InputDispatcher::joyPadDispatch() {}
