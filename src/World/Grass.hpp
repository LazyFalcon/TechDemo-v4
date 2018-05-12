#pragma once
#include "common.hpp"
#include "Logging.hpp"
#include "QuadTree.hpp"
#include "GPUResources.hpp"
#include <queue>

const u32 NO_OF_GRASS_PATCHES_IN_FIELD = 80u;
// const u32 NO_OF_GRASS_PATCHES_IN_FIELD = 25u;
const u32 MAX_FIELD_COUNT = 3048u;
// const u32 MAX_FIELD_COUNT = 2048u;
const i32 LOD_OUT_OF_VIEW = -1;
typedef std::uniform_real_distribution<float> distr;

class ResourceLoader;
class Sampler2D;
class Yaml;

struct SingleGrassPatch
{
    glm::vec4 position;
    // u32 baseColor; // :D
    float size;
    float fallof;
};

typedef std::array<SingleGrassPatch, NO_OF_GRASS_PATCHES_IN_FIELD> GrassPatchPositions;

class GrassField
{
public:
    GrassField(i32 fieldInChunkId, QTNode *owner) : fieldInChunkId(fieldInChunkId),  owner(owner), lod(LOD_OUT_OF_VIEW) {
        owner->payload.grassData->fields[fieldInChunkId] = this;
        switch(fieldInChunkId){
            case 0u : {
                center = owner->center + owner->dimensions*glm::vec4(-0.25, 0.25, 0, 0);
                break;
            }
            case 1u : {
                center = owner->center + owner->dimensions*glm::vec4(0.25, 0.25, 0, 0);
                break;
            }
            case 2u : {
                center = owner->center + owner->dimensions*glm::vec4(0.25, -0.25, 0, 0);
                break;
            }
            case 3u : {
                center = owner->center + owner->dimensions*glm::vec4(-0.25, -0.25, 0, 0);
                break;
            }
            default : error("bad field id received:"s, fieldInChunkId);
        };
    }
    // TODO: why statics? refactor to use common object
    static void cleanup();
    static void init();
    static glm::vec4 size; // size in one direction, assuming regular field shape
    static u32 noOfpatchData;
    static std::vector<GrassPatchPositions> patchData;
    static std::vector<GrassField*> patchDataOwner;

    glm::vec4 center;
    i32 lod;
    void remove();
    bool updateLod(const glm::vec4 &eye, QuadTree &QT);
    bool operator == (const GrassField &gf){
        return &gf == this;
    }
private:
    QTNode *owner;
    std::array<i32, 4> fieldIds {{-1, -1, -1, -1}}; // form lowest to highest level
    i32 fieldInChunkId;

    void plantGrass(i32 lod, QuadTree &QT);

};

class Grass
{
public:
    Grass(QuadTree &qt) : QT(qt) {}
    void initVBO();

    void update(glm::vec4 eyePos);
    void loadData(ResourceLoader &loader, const Yaml &cfg);

    Image texture;
    Mesh mesh {};
    VAO vao;
    u32 positionBuffer;

    u32 getPatchCount(){
        return std::min(GrassField::noOfpatchData, MAX_FIELD_COUNT)*NO_OF_GRASS_PATCHES_IN_FIELD;
    }

private:
    void updateBuffer();
    std::list<GrassField> grassFields;


    glm::vec4 eyePosition;
    std::shared_ptr<Sampler2D> densitySampler;
    QuadTree &QT;
};
