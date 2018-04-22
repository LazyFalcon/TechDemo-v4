#pragma once
#include "GPUResources.hpp" // FIX: really?
#include "ObjectBatchedRender.hpp"
#include "RenderQueue.hpp"

class Camera;
class GraphicDataCollector;
class ObjectBatchedRender;
class ObjectBatchedRender;
class Scene;
class Track;

class GraphicDataCollector
{
public:
    virtual ~GraphicDataCollector() = default;
    virtual void addSelfToQueue() = 0;
};

class GraphicComponent
{
public:
    std::vector<unique_ptr<GraphicDataCollector>> entitiesToDraw;
    void addSelfToQueue(){
        for(auto &it : entitiesToDraw){
            it->addSelfToQueue();
        }
    }
};

class SkinnedMesh : public GraphicDataCollector
{
public:
    SkinnedMesh(){}
    Mesh mesh;
    VAO vao;
    std::vector<glm::mat4> glmTransforms;
    void addSelfToQueue(){
        RenderQueue::insert(this);
    }
};

class ArmoredVehicleTracks : public GraphicDataCollector
{
public:
    ArmoredVehicleTracks(Track &track) : track(track){}
    Track &track;
    Mesh mesh;
    VAO vao;
    glm::mat4 baseTransform;
    void addSelfToQueue(){
        RenderQueue::insert(this);
    }
};
