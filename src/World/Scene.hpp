#pragma once
#include "SceneGraph.hpp"

class AI;
class Atmosphere;
class Camera;
class CameraController;
class CameraControllerFactory;
class Environment;
class Foliage;
class FreeCamController;
class GeoTimePosition;
class Grass;
class PhysicalWorld;
class Starfield;
class Sun;
class Terrain;
struct Yaml;

struct SpawnPoint
{
    std::string name;
    glm::mat4 transform;
};

struct Scene : private boost::noncopyable
{
    Scene(PhysicalWorld &physics, CameraControllerFactory& camFactory);
    ~Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    PhysicalWorld& physics;
    std::unique_ptr<Atmosphere> atmosphere;
    std::unique_ptr<Environment> environment;
    std::unique_ptr<Foliage> foliage;
    std::unique_ptr<GeoTimePosition> geoTimePosition;
    std::unique_ptr<Grass> grass;
    std::unique_ptr<SceneGraph> graph;
    std::unique_ptr<Starfield> starfield;
    std::unique_ptr<Sun> sun;
    std::unique_ptr<Terrain> terrain;
    std::vector<SpawnPoint> spawnPoints;
    CameraControllerFactory& camFactory;
    std::vector<std::shared_ptr<FreeCamController>> freeCams;

    std::vector<std::shared_ptr<AI>> m_friendlyBots;
    std::vector<std::shared_ptr<AI>> m_hostileBots;

    bool load(const std::string &name);
    void update(float dt, Camera &camera);
    void extractSpawnPoints(const Yaml& yaml);
    void extractCameras(const Yaml& yaml);

    template<typename T>
    SampleResult sample(T position){
        if(not graph) return {};
        return graph->sample(position);
    }

    glm::vec4 getSceneDimension();
    std::vector<std::shared_ptr<AI>>& getHostiles(){
        return m_hostileBots;
    }

};
