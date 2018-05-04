#pragma once
#include "common.hpp"
#include <list>
#include <map>
#include <memory>

class Input;

class InputDispatcher
{
private:
    std::multimap<std::string, std::string> configuredActions;
    std::list<int> currentlyPressedKeys;
    int currentModifierKey;
public:
    std::shared_ptr<Input> root;
    Input *active;

    void setBinding(const std::string& combination);

    auto getBinding(const std::string& function) -> decltype(configuredActions.equal_range(function)){
        return configuredActions.equal_range(function);
    }

    void execute(int k, int a, int m);
    void refresh();
    void scrollCallback(double dx, double dy);
    void keyCallback(int key, int action, int mods);
    void mouseButtonCallback(int button, int action, int mods);
    void mousePosition(float x, float y);
    void mouseMovement(float x, float y);
    void joyPadDispatch();
};
