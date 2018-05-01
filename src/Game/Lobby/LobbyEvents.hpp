#pragma once
#include "Events.hpp"

struct StartGame : public Event
{
    bool handle(App &app);
};

struct ExitGame : public Event
{
    bool handle(App &app);
};
