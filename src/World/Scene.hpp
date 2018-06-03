#pragma once
#include "SceneGraph.hpp"

class Atmosphere;
class Camera;
class Environment;
class Grass;
class PhysicalWorld;
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
    Scene(PhysicalWorld &physics) : physics(&physics){}
    ~Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    std::shared_ptr<Atmosphere> atmosphere;
    std::shared_ptr<Environment> environment;
    std::shared_ptr<Grass> grass;
    std::shared_ptr<SceneGraph> graph;
    std::shared_ptr<Sun> sun;
    std::shared_ptr<Starfield> starfield;
    std::shared_ptr<Terrain> terrain;
    std::shared_ptr<Foliage> foliage;
    PhysicalWorld *physics { nullptr };
    std::shared_ptr<GeoTimePosition> geoTimePosition;

    bool load(const std::string &name, Yaml &cfg);
    void update(float dt, Camera &camera);

    template<typename T>
    SampleResult sample(T position){
        if(not graph) return {};
        return graph->sample(position);
    }
};
