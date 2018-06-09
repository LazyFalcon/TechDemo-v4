#pragma once
#include <list>
#include <map>
#include <memory>

class Input;

class InputDispatcher
{
private:
    std::map<std::string, std::string> m_configuredActions;
    std::list<int> currentlyPressedKeys;
    // TODO: change shared_ptr on weak_ptr or raw pointer, storing input here cannot block destroying of input
    std::list<std::shared_ptr<Input>> m_activeInputHandlers;
    int m_currentModifierKey;
public:
    std::shared_ptr<Input> createNew(const std::string&);
    void remove(Input*);

    InputDispatcher& defineBinding(const std::string& name, const std::string& keys);
    const std::string& getDefined(const std::string& name);

    void execute(int k, int a, int m);
    void heldUpKeys();
    void scrollCallback(double dx, double dy);
    void keyCallback(int key, int action, int mods);
    void mouseButtonCallback(int button, int action, int mods);
    void mousePosition(float x, float y);
    void mouseMovement(float x, float y);
    void joyPadDispatch();
};
