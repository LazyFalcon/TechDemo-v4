#pragma once
#include "Events.hpp"

struct StartPlayground : public Event
{
    StartPlayground(std::string sceneName) : sceneName(sceneName){}
    bool handle(App &app);
    std::string sceneName;
};

struct ExitPlayground : public Event
{
    bool handle(App &app);
};
