#pragma once
#include "SceneGraph.hpp"

class Atmosphere;
class Camera;
class CameraController;
class Environment;
class Foliage;
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
    Scene(PhysicalWorld &physics);
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

    bool load(const std::string &name);
    void update(float dt, Camera &camera);
    void extractSpawnPoints(const Yaml& yaml);

    struct {
        struct {
            glm::vec4 lookDirection;
            glm::vec4 position;
        } player;

        std::vector<std::shared_ptr<CameraController>> cameras;

    } output;


    template<typename T>
    SampleResult sample(T position){
        if(not graph) return {};
        return graph->sample(position);
    }
};
