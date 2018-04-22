#pragma once
#include "SceneGraph.hpp"

class Atmosphere;
class Camera;
class Environment;
class Grass;
class PhysicsWorld;
class Sun;
class Starfield;
class Terrain;
class Foliage;
class GeoTimePosition;
struct Yaml;

// TODO: add interfaces and use unique_ptrs
struct Scene
{
    Scene(){}
    Scene(PhysicsWorld &physics) : physics(&physics){}
    ~Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    shared_ptr<Atmosphere> atmosphere;
    shared_ptr<Environment> environment;
    shared_ptr<Grass> grass;
    shared_ptr<SceneGraph> graph;
    shared_ptr<Sun> sun;
    shared_ptr<Starfield> starfield;
    shared_ptr<Terrain> terrain;
    shared_ptr<Foliage> foliage;
    PhysicsWorld *physics { nullptr };
    shared_ptr<GeoTimePosition> geoTimePosition;

    bool load(const std::string &name, Yaml &cfg);
    void update(float dt, Camera &camera);

    template<typename T>
    SampleResult sample(T position){
        if(not graph) return {};
        return graph->sample(position);
    }
};
