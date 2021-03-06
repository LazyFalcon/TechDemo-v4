#pragma once
#include "Foliage.hpp"
#include "GPUResources.hpp"
#include "base-of-game-object.hpp"

static const i32 NoOfLevels = 5;
namespace camera
{
class Frustum;
}
class btBvhTriangleMeshShape;
class btRigidBody;
class btTriangleMesh;
struct VertexWithMaterialData;
class PhysicalWorld;
template<typename VertexFormat>
class ModelLoader;
class Yaml;

class Cell : public BaseOfGameObject
{
public:
    Cell() : BaseOfGameObject(GameType::Cell) {}
    glm::vec4 position;
    glm::vec4 size;

    i32 id;
    i32 level;

    bool hasTerrain {false};
    Mesh terrainMesh {};
    btRigidBody* cellBoxCollider {nullptr};
    std::vector<ObjectHandle> objects;

    SampleResult sample(glm::vec2 position) {
        // allhitRaycast i porownanie pointerow
        return {};
    }

    void addToShadowCastingList(model::Collection&) override;
    void addToSceneVisibleList(visuals::PreparedScene&) override;
    btRigidBody* getCollider() override {
        return cellBoxCollider;
    }
};

constexpr i32 getCellCount(i32 levels) {
    i32 out(0);
    if(levels < 0)
        return 0;
    for(auto i = 0; i <= levels; i++) { out += pow(4, i); }
    return out;
}
/*
? How Culling is performed?
* Fully with Bullet support - bullet will test all objects against frustum and loop over this list, adding them all to rendering queue
* Also some static objects will be pulled from cells
* Let's belive in bullet performance in doing such culling, of course algorithm with cells could be mode efficient, but also tooks time to implement, and bullet can be run in another thread :D
* Assuming scene will no be too big

* Option for querying objects for custom colliders: shadows, probes
*/
class SceneGraph
{
public:
    glm::vec4 size {1500, 1500, 0, 0}, min, max;
    glm::vec4 center {0, 0, 0, 0};
    glm::vec2 nodes {32, 32};
    glm::vec2 cellSize;
    glm::vec2 cellsInTheScene;

    Cell* root;
    std::vector<Cell> cells;

    SceneGraph(PhysicalWorld& physics);
    std::vector<i32> getVisibleCells();

    i32 idFromPosition(glm::vec2 position, i32 level = NoOfLevels);
    SampleResult sample(glm::vec2 position) {
        return sample(glm::vec4(position, 1000, 0));
    }
    SampleResult sample(glm::vec4 position);

    void initAndLoadMap(const Yaml& yaml);
    void cullCells(const camera::Frustum& frstum);
    void findObjectsOutsideFrustumThatCastShadows(const camera::Frustum& frustum, glm::vec4 lightDirection);

    void insertObject(ObjectHandle obj, const glm::vec4& position);
    glm::vec4 getDimensions() {
        return size;
    }

    // wszystkie lodLevele na raz, posortowane po lod i odleglosci
    std::vector<i32> visibleCells;
    std::vector<i32> addedCells;
    std::vector<i32> removedCells;
    VAO vao;
    btRigidBody* collider {nullptr};
    btBvhTriangleMeshShape* colliderShape {nullptr};
    btTriangleMesh* colliderMesh {nullptr};
    btSphereShape* defaultSphereCollider {nullptr};

private:
    PhysicalWorld& physics;
    Cell* findCellUnderPosition(const glm::vec4& pos);
    void initCellsToDefaults();
    void loadMap(const std::string& mapConfigDir);
    void findObjectsInsideFrustum(const camera::Frustum& frustum);
    void loadCollider(ModelLoader<VertexWithMaterialData>& loader, const std::string& name);
    btRigidBody* createSimpleCollider(glm::vec4 pos, glm::vec3 dim);
};
