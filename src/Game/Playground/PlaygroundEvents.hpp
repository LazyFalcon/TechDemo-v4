#pragma once
#include "Events.hpp"

struct StartPlayground : public Event
{
    StartPlayground(std::string vehicleName, std::string sceneName) : vehicleName(vehicleName), sceneName(sceneName){}
    bool handle(App &app);
    std::string vehicleName;
    std::string sceneName;
};

struct ExitPlayground : public Event
{
    bool handle(App &app);
};
