#pragma once
#include "GPUResources.hpp"
#include "Foliage.hpp"
#include "Object.hpp"

static const i32 NoOfLevels = 5;
class Frustum;
class btRigidBody;
class btBvhTriangleMeshShape;
class btTriangleMesh;
class ModelLoader;
class PhysicalWorld;

class Cell
{
public:
    glm::vec4 position;
    glm::vec4 size;

    i32 id;
    i32 level;

    bool hasTerrain {false};
    Mesh terrainMesh {};
    btRigidBody *cellBoxCollider {nullptr};
    std::vector<Foliage> foliage;
    std::vector<i32> objects;

    SampleResult sample(glm::vec2 position){
        // allhitRaycast i porownanie pointerow
        return {};
    }
};

constexpr i32 getCellCount(i32 levels){
    i32 out(0);
    if(levels < 0) return 0;
    for(auto i=0; i<=levels; i++){
        out += pow(4, i);
    }
    return out;
}

class SceneGraph
{
public:
    glm::vec4 size {1500, 1500,0,0};
    glm::vec4 center {0,0,0,0};
    glm::vec2 nodes {32, 32};

    Cell root;
    std::vector<Cell> cells;

    std::unordered_map<ObjectID, Object> objects; // na razie uMap wystarczy, najwyżej zamieni sie potem na jakąś zabawę z przesuwanie na wektorze
    std::map<Type, std::vector<Object>> visibleObjects;

    SceneGraph(PhysicalWorld &physics);
    std::vector<i32> getVisibleCells();

    i32 idFromPosition(glm::vec2 position, i32 level = NoOfLevels);
    SampleResult sample(glm::vec2 position){
        return sample(glm::vec4(position, 1000, 0));
    }
    SampleResult sample(glm::vec4 position);

    void create();
    void loadMap(const std::string &mapConfigPath);

    void cullCells(const Frustum &frstum);

    // wszystkie lodLevele na raz, posortowane po lod i odleglosci
    std::vector<i32> visibleCells;
    std::vector<i32> addedCells;
    std::vector<i32> removedCells;
    VAO vao;
    btRigidBody *collider {nullptr};
    btBvhTriangleMeshShape *colliderShape {nullptr};
    btTriangleMesh *colliderMesh {nullptr};
    btSphereShape *defaultSphereCollider {nullptr};
private:
    PhysicalWorld &physics;
    void cullWithPhysicsEngine(const Frustum &frustum);
    void diffBetweenFrames();
    void setLodForVisible(glm::vec4 eye);
    void loadCollider(ModelLoader &loader, const std::string &name);
    btRigidBody* createSimpleCollider(glm::vec4 pos, glm::vec3 dim);
};
