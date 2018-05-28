#pragma once
#include "Events.hpp"

struct StartPlayground : public Event
{
    bool handle(App &app);
};

struct ExitPlayground : public Event
{
    bool handle(App &app);
};
