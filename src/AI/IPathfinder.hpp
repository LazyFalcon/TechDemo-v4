#pragma once

#include "Waypoint.hpp"

using Waypoints = std::vector<Waypoint>;

class IPathfinder
{
public:
    virtual ~IPathfinder() = default;
    virtual Waypoints calculate(Waypoint from, Waypoint to) = 0;
    virtual void preprocessMap() = 0;
};
