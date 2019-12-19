#pragma once
#include "GPUResources.hpp" // FIX: really?
#include "Logger.hpp"
#include "visuals-prepared-scene.hpp"

namespace model
{
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
    void toBeRendered(RenderCommand& into) {
        for(auto& it : entitiesToDraw) { it->toBeRendered(into); }
    }
};
// ? make movable?
class Skinned : public GraphicDataCollector
{
public:
    Mesh mesh;
    VAO vao;
    std::vector<glm::mat4> bones;
    // ? leave this to user
    void toBeRendered(RenderCommand& into) {
        into.skinned.push_back(this);
    }
};

class TrackLink : public GraphicDataCollector
{
public:
    Mesh mesh;
    VAO vao;
    glm::mat4 baseTransform;
    std::vector<glm::vec2> links;
    void toBeRendered(RenderCommand& into) {
        into.trackLinks.push_back(this);
    }
};
}