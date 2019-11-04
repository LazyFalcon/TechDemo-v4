#pragma once
#include "core.hpp"
#include "IPathfinder.hpp"

class NoPathfinder : public IPathfinder
{
public:
    Waypoints calculate(Waypoint from, Waypoint to) override {
        float distance = glm::distance(from.position, to.position);
        float n = std::ceil(distance);
        glm::vec4 v = (to.position - from.position) / distance;

        Waypoints out;
        for(float i = 0; i < n; i++) out.push_back({from.position + v * i, to.direction, to.velocity});

        return out;
    }
    void preprocessMap() {
        console.log("lol, nothing happens");
    }
};
