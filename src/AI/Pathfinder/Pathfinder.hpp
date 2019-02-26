#pragma once
#include "IPathfinder.hpp"

class Scene;
class Context;

class Pathfinder : public IPathfinder
{
private:
    Scene& m_scene;
    Context& m_context;

public:
    Pathfinder(Scene& scene, Context& context): m_scene(scene), m_context(context){}
    Waypoints calculate(Waypoint from, Waypoint to);
    void preprocessMap();
};
