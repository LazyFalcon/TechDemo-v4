#pragma once
#include "camera-frustum.hpp"
#include "QTPayload.hpp"
#include "BaseStructs.hpp"

struct LightSource;
class Terrain;
class PhysicalWorld;
struct Yaml;

enum LodLevel {
    DUMMY = -1, LOD_0 = 0, LOD_1=1, LOD_2=2, LOD_3=3, OCCLUDED, OUT_OF_VIEW,
};

struct QTNode
{
    glm::vec4 center;
    glm::vec4 dimensions;

    u32 id;
    u32 parentId;
    u32 lodLevel;

    bool occluded;
    bool visible { false };
    float min, max;
    float gridSize;

    QTPayload payload;
    float *dataStart;
    std::array<QTNode*, 4> children;
    QTNode* parent;
    QTNode& operator [] (u32 i){
        return *children[i];
    }
    QTNode& operator () (u32 i, u32 j);

    std::vector<u32> objects; // assuming that object are static and never removed
    std::vector<std::shared_ptr<LightSource>> lightSources; // assuming that object are static and never removed

    u32 testSphereAgainsFrustum(const camera::Frustum &frustum) const;
    u32 testAABBAgainsFrustum(const camera::Frustum &frustum) const;
    bool insideFlat(const glm::vec4&) const;
    glm::vec4 distanceTo(const glm::vec4&) const;
    glm::vec4 distanceToBorders(const glm::vec4&) const;
    bool areAnyChildVisible() const {
        return children[0]->visible or children[1]->visible or children[2]->visible or children[3]->visible;
    }

    SampleResult sampleHeight(glm::vec4) const ;
};

struct LodLevelContainer
{
    std::vector<QTNode*> visibleNodes;
    std::vector<QTNode*> addedNodes;
    std::vector<QTNode*> removedNodes;
    std::vector<glm::vec4> renderData;
};

class QuadTree
{
public:
    QuadTree(PhysicalWorld &p) : physics(p){}
    void init(const Yaml &cfg);
    void buildQTNode(QTNode &qtNode, u32 lodLevel);
    void buildQTNodes(Terrain &terrain);
    void buildQTNodes(QTNode &qtNode, u32 lodLevel, Terrain &terrain);
    void buildQTLeaf(QTNode &qtNode, Terrain &terrain);
    void update(const camera::Frustum &frustum);
    void registerInBullet(QTNode &qtNode);

    LodLevel findLodLevel(QTNode &node, const camera::Frustum &frustum);
    void frustumCull(QTNode &node, const camera::Frustum &frustum);
    void addToVisible(QTNode &node, LodLevel level);
    void removevisible(QTNode &qtNode);
    void recalculateNodeZPosition();
    std::pair<float, float> recalculateNodeZPosition(QTNode &node);

    std::vector<u32> getVisibleObjects();

    // utils
    SampleResult sample(glm::vec2 position){
        return sample(glm::vec4(position, 0, 0));
    }
    SampleResult sample(glm::vec4 position);

    SampleResult sampleTerrain(glm::vec2 position){
        return sampleTerrain(glm::vec4(position, 0, 0));
    }
    SampleResult sampleTerrain(glm::vec4 position);
    SampleResult sampleHeight(glm::vec4, const QTNode*) const ;
    u32 lod0Count(){
        return pow(4, levels);
    }
    QTNode& findTouchedNodes(glm::vec4 p, float radius);

    u32 maxLodLevel;
    std::array<LodLevelContainer, 5> lodLevels;

    std::vector<QTNode> QTNodes;

    bool enableOC;
    float FoliageDensityMap;

    u32 levels;
    glm::ivec2 nodes; // qtSize;
    glm::vec2 size; // qtDimension;
    glm::vec2 chunkSize; // chunkDimension;

    PhysicalWorld &physics;
private:
    u32 qtNodeIndex {0}; // used during qt bulding
};
