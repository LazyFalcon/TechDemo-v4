#pragma once
#include "common.hpp"
#include "input-utils.hpp"

using Lambda = std::function<void(void)>;
using Lambda2F = std::function<void(float,float)>;

struct Action
{
    std::function<void(void)> func;
    void operator () (){
        if(func) {
            // log("function", name);
            func();
        }
    }
};

struct Action2F
{
    std::function<void(float, float)> func;
    void operator () (float x, float y){
        if(func) func(x, y);
    }
};

class InputDispatcher;

class Input
{
private:
    std::multimap<u32, Lambda> actions; // TODO: maybe unordered_ ?
    std::multimap<u32, Lambda2F> actions2f;
    InputDispatcher& inputDispatcher;
    std::vector<std::shared_ptr<Input>> children;
    std::function<void(const std::string&)> m_forwardAction;
    std::string m_name;
    KeyActionModifier m_currentBinding {};
public:
    Input(InputDispatcher& inputDispatcher, std::string name="") : inputDispatcher(inputDispatcher), m_name(name){}
    ~Input();
    bool execute(int k, int a, int m);
    bool executeTwoArgs(int arg, float x, float y);
    void activate();
    void deactivate();

    Input& action(const std::string&);
    Input& defined(const std::string&);
    Input& forward(std::function<void(const std::string&)>&&);
    Input& name(const std::string&);
    Input& on(Lambda&&);
    Input& hold(Lambda&&);
    Input& off(Lambda&&);
    Input& on(Lambda2F&&);

    InputDispatcher& getDispatcher(){
        return inputDispatcher;
    }

    bool active {false};
};

using InputContextPtr = std::shared_ptr<Input>;
