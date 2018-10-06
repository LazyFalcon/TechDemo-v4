#pragma once
#include "BaseStructs.hpp"
#include "QuadTree.hpp"

#include "GPUResources.hpp"
#include "Sampler2D.hpp"

// https://www.opengl.org/sdk/docs/man/html/glMultiDrawElements.xhtml
/**
  *  At now render entities simply, but prepare things to be rendered with multiDraw*
  *  - mesh info and number of instances
  *  - big chunk of data containing matrices and IDs of textures
  *  - textureArray with textures
  *  - VAO with data
  *
  *  * differ on trees(big objects, many textures) and bushes(small objects, one texture, sometimes shared between)
  *  * on begin let's assume that we have one texture per object
  *
  * */

class Sampler2D;
class btRigidBody;
 class PhysicalWorld;

/**
  *  To plant trees we need
  *  - quadTree
  *  - density map - TODO: cpu sampler
  *  - luck
  *
  * */
struct TreeMesh
{
    Mesh crown;
    Mesh trunk;
};
struct FoliageModel
{
    std::string name;
    std::array<TreeMesh, 5> mesh;
    glm::vec4 aabb;
};
struct FoliageUniform
{
    glm::mat4 transform;
    glm::vec4 motion;
};
struct FoliagePhysics
{
    QTNode *owner { nullptr };
    btRigidBody *rgBody { nullptr };
    btGeneric6DofSpring2Constraint *rgJoint { nullptr };
    FoliageModel *model { nullptr };

    bool cracked { false };
};

struct RenderData
{
    struct {
        std::vector<u32> counts;
        std::vector<u32> start;
    } crown;
    struct {
        std::vector<u32> counts;
        std::vector<u32> start;
    } trunk;
    std::vector<FoliagePhysics> info; // never zero
    std::vector<FoliageUniform> uniforms;
    std::vector<u32> shadowCasters;
    glm::vec4 bbMin, bbMax;
    u32 count {0};
    u32 next(){
        if(count == crown.counts.size()){
            crown.counts.resize(crown.counts.size()+10);
            crown.start.resize(crown.start.size()+10);
            trunk.counts.resize(trunk.counts.size()+10);
            trunk.start.resize(trunk.start.size()+10);
            info.resize(info.size()+10);
            uniforms.resize(uniforms.size()+10);
        }
        return count++;
    }

    glm::vec4 oscillation {};
    u32 push(FoliageModel &foliage, QTNode *id, const FoliageUniform &f){
        u32 pos = next();
        crown.counts[pos] = foliage.mesh[0].crown.count;
        crown.start[pos] = foliage.mesh[0].crown.begin;
        trunk.counts[pos] = foliage.mesh[0].trunk.count;
        trunk.start[pos] = foliage.mesh[0].trunk.begin;
        uniforms[pos] = f;
        info[pos].owner = id;
        info[pos].model = &foliage;

        return pos;
    }
    void cleanup(std::vector<u32> FoliageData::*member);
    void updatePhysics(glm::vec4 reference, PhysicalWorld &p);
    static std::function<void(btRigidBody *&body, btGeneric6DofSpring2Constraint *constraint)> removeTreeFromPhysics;
};

class FoliageDensityMap
{
public:
    FoliageDensityMap(glm::vec2 worldSize, glm::ivec2 nodes);
    u8 sample(glm::vec2);
    void generate();
    void reloadIfNeeded();
    void drawDebug();

    u32 texture {0};
private:
    void upload();
    u32 textureId {0};
    Sampler<u8> sampler;
    float octaves {5.f};
    float persistence {0.0f};
    float scale {0.05f};
    float seedOffset {10.8f};
    float seed {3.8f};
    u32 threshold {100};
    bool showDebug {false};
    bool isReloadNeeded {false};
};

class Foliage
{
public:
    ~Foliage();
    u32 textureAtlasID{0};
    VAO meshVAO;

    Foliage(QuadTree &qt, PhysicalWorld &p) : QT(qt), physics(p), densityMap(qt.size, qt.nodes){}
    bool load(const Yaml &config);
    void update(glm::vec4 reference);
    RenderData& getRenderData(){
        return treeBatchData;
    }
    RenderData treeBatchData;
    RenderData shrubBatchData;

private:
    std::vector<FoliageModel> trees;
    std::vector<FoliageModel> shrubs;
    std::vector<FoliageModel> plants;
    QuadTree &QT;
    PhysicalWorld &physics;
    std::shared_ptr<Sampler2D> densitySampler;
    FoliageDensityMap densityMap;
    std::vector<std::string> prepareAtlas();

    void plantTrees(FoliageData &data, QTNode &node, QuadTree &QT);

    // rnd
};
