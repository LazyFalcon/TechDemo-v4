#pragma once
#include "Events.hpp"

struct StartLobby : public Event
{
    bool handle(App& app);
};

struct ExitGame : public Event
{
    bool handle(App& app);
};
