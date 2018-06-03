#pragma once
#include "GPUResources.hpp"
#include "BaseStructs.hpp"
#include "LightSource.hpp"
#include "SceneGraph.hpp"

class btRgBody;
class btCollisionShape;
class Yaml;
class PhysicalWorld;

namespace projectiles {class Projectile;}

class EnviroEntity
{
public:
    struct {
        Mesh mesh;
        u32 lodLevel;

        bool glossy;
        float glossyEnergy;
        glm::vec4 color;
    } graphic;
    struct {
        glm::vec4 position;
        glm::mat4 transform;
        btRigidBody *rgBody {nullptr};
    } physics;

    int id {0};

    bool processHit(projectiles::Projectile&){
        return true;
    };
    void printStatus(){};
    void update(float dt);
};


class Environment
{
public:
    Environment(SceneGraph &graph, PhysicalWorld *physics) : graph(graph), physics(physics) {}

    void load(const std::string &dirPath);

    LightSourcesContainer lightSources;
    std::vector<EnviroEntity> glossyObjects;
    std::vector<EnviroEntity> entities;
    EnviroEntity& getObject(u32 id){
        return entities[id];
    }
    VAO vao;
private:
    SceneGraph &graph;
    PhysicalWorld *physics;

    // std::map<std::string, Mesh> meshes;
    // std::map<std::string, btCollisionShape*> collsionShapes;

    void loadMesh(ModelLoader &modelLoader, EnviroEntity &e, const Yaml &yaml);
    void createObject(const Yaml &cfg);
    btRigidBody* createRgBody(const Yaml &bodyConf);
    void insertObjectToQt(u32 id);
};
