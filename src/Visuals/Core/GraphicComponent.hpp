#pragma once
#include "GPUResources.hpp" // FIX: really?
#include "Logger.hpp"
#include "RenderDataCollector.hpp"

namespace camera
{
class Camera;
}
class GraphicDataCollector;
class Scene;
class Track;

class GraphicDataCollector
{
public:
    virtual ~GraphicDataCollector() = default;
    virtual void toBeRendered() = 0;
};

class GraphicComponent
{
public:
    std::vector<std::shared_ptr<GraphicDataCollector>> entitiesToDraw;
    void toBeRendered() {
        for(auto& it : entitiesToDraw) { it->toBeRendered(); }
    }
};

class SkinnedMesh : public GraphicDataCollector
{
public:
    SkinnedMesh() {}
    Mesh mesh;
    VAO vao;
    std::vector<glm::mat4> bones;
    void toBeRendered() {
        RenderDataCollector::insert(this);
    }
};

class ArmoredVehicleTracks : public GraphicDataCollector
{
public:
    ArmoredVehicleTracks(Track& track) : track(track) {}
    Track& track;
    Mesh mesh;
    VAO vao;
    glm::mat4 baseTransform;
    void toBeRendered() {
        RenderDataCollector::insert(this);
    }
};
