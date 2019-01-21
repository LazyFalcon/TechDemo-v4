#pragma once

struct Waypoint
{
    glm::vec4 position;
    glm::vec4 direction;
    float velocity;
};
using Waypoints = std::vector<Waypoint>;

class IPathfinder
{
public:
    virtual Waypoints calculate(Waypoint from, Waypoint to) = 0;
};