#pragma once
#include "input-mouse-state.hpp"
#include "input-utils.hpp"

using Lambda = std::function<void(void)>;
using Lambda2Inputs = std::function<void(float, float)>;

template<typename ActionSignature>
struct Action
{
    Action(ActionSignature f) : func(f) {}
    ActionSignature func;
    u32 lastTimeUsed;
    u32 noOfRepeats;
    template<typename... Args>
    void operator()(Args&... args) {
        if(func)
            func(args...);
    }
};

class InputDispatcher;

class Input
{
private:
    std::multimap<u32, Action<Lambda>> actions; // TODO: maybe unordered_ ?
    std::multimap<u32, Action<Lambda2Inputs>> actions2f;
    InputDispatcher& inputDispatcher;
    std::vector<std::shared_ptr<Input>> children;
    std::function<void(const std::string&)> m_forwardAction;
    std::string m_name;
    KeyActionModifier m_currentBinding {};

public:
    Input(InputDispatcher& inputDispatcher, std::string name = "") : inputDispatcher(inputDispatcher), m_name(name) {}
    ~Input();
    bool execute(int k, int a, int m, u32 currentTime);
    bool executeTwoArgs(int arg, float x, float y);
    void startHoldKey(int k, int m, u32 currentTime);
    void releaseHeldKey(int k, int m);
    void activate();
    void deactivate();

    const InputMouseState& getMouseState() const;
    // todo: maybe add something like context? to not perform actions when other window is focused? Like in vs code
    Input& action(const std::string&);
    Input& defined(const std::string&);
    Input& forward(std::function<void(const std::string&)>&&);
    Input& name(const std::string&);
    Input& on(Lambda&&);
    Input& hold(Lambda&&);
    Input& off(Lambda&&);
    Input& on(Lambda2Inputs&&);

    InputDispatcher& getDispatcher() {
        return inputDispatcher;
    }

    bool active {false};

    u32 repeatTresholdTime {555};
    struct
    {
        u32 noOfRepeats; // * each key, .on or .off #repeats increases if fitrs in repeatTresholdTime since last
        u32 onHoldTime;  // * valid for hold keys, time since key was pressed
        bool release;    // * release key from processing
    } currentKey {};
};

using InputContextPtr = std::shared_ptr<Input>;
