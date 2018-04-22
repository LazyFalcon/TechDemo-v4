#pragma once
#include "Includes.hpp"

struct TerrainData;
struct GrassData;
struct FoliageData;
// struct Environmentdata;

struct QTPayload
{
    unique_ptr<TerrainData> terrainData;
    unique_ptr<GrassData> grassData;
    unique_ptr<FoliageData> foliageData;
    // unique_ptr<Environmentdata> environmentdata;
};

struct TerrainData
{
    void *rgShape;
    float *data;
    float min;
    float max;
    float heightOffset;
    u32 size;
    float operator () (u32 x, u32 y) const {
        return data[x + y*size];
    }
    float& operator () (u32 x, u32 y){
        return data[x + y*size];
    }
};
class GrassField;
struct GrassData
{
    std::array<GrassField*, 4> fields;
};
struct FoliageData
{
    std::vector<u32> treeID;
    std::vector<u32> shrubID;
};
