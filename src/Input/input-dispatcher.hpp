#pragma once
#include "common.hpp"
#include <list>
#include <map>
#include <memory>

class Input;

class InputDispatcher
{
private:
    std::map<std::string, std::string> m_configuredActions;
    std::list<int> currentlyPressedKeys;
    std::vector<std::shared_ptr<Input>> m_activeIputHandlers;
    int m_currentModifierKey;
public:
    std::shared_ptr<Input> createNew(const std::string&);

    InputDispatcher& defineBinding(const std::string& name, const std::string& keys);
    const std::string& getDefinied(const std::string& name);

    void execute(int k, int a, int m);
    void heldUpKeys();
    void scrollCallback(double dx, double dy);
    void keyCallback(int key, int action, int mods);
    void mouseButtonCallback(int button, int action, int mods);
    void mousePosition(float x, float y);
    void mouseMovement(float x, float y);
    void joyPadDispatch();
};
