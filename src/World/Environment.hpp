#pragma once
#include "GPUResources.hpp"
#include "BaseStructs.hpp"
#include "LightSource.hpp"
#include "SceneGraph.hpp"
#include "SceneObject.hpp"

class btCollisionShape;
class btRgBody;
struct VertexWithMaterialData;
template<typename VertexFormat>
class ModelLoader;
class PhysicalWorld;
class Yaml;

namespace projectiles {class Projectile;}

class EnviroEntity : public ObjectInterface
{
public:
    std::string name;
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
    void actionVhenVisible() override;
};


class Environment
{
public:
    Environment(SceneGraph &graph, PhysicalWorld &physics) : graph(graph), physics(physics) {}

    void load(const std::string &dirPath);

    LightSourcesContainer lightSources;
    std::vector<EnviroEntity> glossyObjects;
    std::vector<ObjectWrapper<EnviroEntity>> m_entities;
    EnviroEntity& getObject(u32 id){
        return *m_entities[id];
    }
    VAO vao;
private:
    SceneGraph &graph;
    PhysicalWorld &physics;

    // std::map<std::string, Mesh> meshes;
    // std::map<std::string, btCollisionShape*> collsionShapes;

    void loadObject(const Yaml &yaml, ModelLoader<VertexWithMaterialData>& modelLoader);
    void loadLightSource(const Yaml &yaml);

    void loadMesh(ModelLoader<VertexWithMaterialData>& modelLoader, EnviroEntity &e, const Yaml &yaml);
    void createObject(const Yaml &cfg);
    btRigidBody* createRgBody(const Yaml &bodyConf);
    void insertObjectToQt(u32 id);
};
