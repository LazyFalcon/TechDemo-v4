#pragma once
#include "GPUResources.hpp"
#include "BaseStructs.hpp"
#include "LightSource.hpp"
#include "SceneGraph.hpp"
#include "BaseGameObject.hpp"

class btCollisionShape;
class btRgBody;
struct VertexWithMaterialData;
template<typename VertexFormat>
class ModelLoader;
class PhysicalWorld;
class Yaml;

namespace projectiles {class Projectile;}

class EnviroEntity : public BaseGameObject
{
public:
    EnviroEntity() = default;
    EnviroEntity(const std::string& name) : name(name){}
    std::string name;
    struct {
        Mesh mesh;
        u32 lodLevel;

        bool glossy;
        float glossyEnergy;
        glm::vec4 color;
    } graphic {};
    struct {
        glm::vec4 position;
        glm::vec4 dimensions;
        glm::mat4 transform;
        btRigidBody *rgBody {nullptr};
        btCollisionShape  *shape {nullptr};
    } physics {};

    int id {0};

    bool processHit(projectiles::Projectile&){
        return true;
    };
    void printStatus(){};
    void update(float dt);
    void actionWhenVisible() override;
    btRigidBody* getCollider(){
        return physics.rgBody;
    }
};


class Environment
{
public:
    Environment(SceneGraph &graph, PhysicalWorld &physics) : graph(graph), physics(physics) {}

    void load(const std::string &dirPath);

    std::vector<LightSource> m_lights;
    std::vector<EnviroEntity> m_entities;

    void update(float dt);

    std::vector<EnviroEntity*> getObstacles(){
        std::vector<EnviroEntity*> out;
        out.reserve(m_entities.size());
        for(auto&it : m_entities) out.push_back(&it);

        return out;
    }

    EnviroEntity& getObject(u32 id){
        return m_entities[id];
    }
    VAO vao;
private:
    SceneGraph &graph;
    PhysicalWorld &physics;

    void loadObject(const Yaml &yaml, ModelLoader<VertexWithMaterialData>& modelLoader);
    void loadLightSource(const Yaml &yaml);

    void loadVisualPart(ModelLoader<VertexWithMaterialData>& modelLoader, EnviroEntity &e, const Yaml &yaml);
    bool loadPhysicalPart(ModelLoader<VertexWithMaterialData>& modelLoader, EnviroEntity &e, const Yaml &yaml);
    void createSimpleCollider(EnviroEntity &entity);
};
