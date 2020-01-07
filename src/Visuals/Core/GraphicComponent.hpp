#pragma once
#include "GPUResources.hpp" // FIX: really?
#include "Logger.hpp"
#include "RenderStructs.hpp"
#include "visuals-prepared-scene.hpp"

namespace model
{
class GraphicDataCollector
{
public:
    virtual ~GraphicDataCollector() = default;
    // virtual void toBeRendered() = 0;
};

class Skinned;
class TrackLink;

class Collection
{
public:
    Collection(bool isForShadows = false) : id(s_id++), isForShadows(isForShadows) {}
    static uint s_id;
    uint id;
    bool isForShadows;
    std::vector<model::Skinned*> skinned;
    std::vector<model::TrackLink*> trackLinks;
    // std::vector<ArmoredVehicleTracks*> tracks;
    // CommandArray dummy;
    // CommandArray terrain;
    // CommandArray foliage;
};

// ? make movable?
class Skinned : public GraphicDataCollector
{
public:
    Mesh mesh;
    VAO vao;
    std::vector<glm::mat4> bones;
    // ? leave this to user
    // void toBeRendered(RenderCommand& into) {
    //     into.skinned.push_back(this);
    // }
};

class TrackLink : public GraphicDataCollector
{
public:
    Mesh mesh;
    VAO vao;
    glm::mat4 baseTransform;
    std::vector<glm::vec2> links;
    // void toBeRendered(RenderCommand& into) {
    //     into.trackLinks.push_back(this);
    // }
};

}
