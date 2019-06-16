#pragma once
#include "GPUResources.hpp" // FIX: really?
#include "RenderDataCollector.hpp"
#include "Logger.hpp"

class Camera;
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
    void toBeRendered(){
        for(auto &it : entitiesToDraw){
            console.clog(__PRETTY_FUNCTION__);
            it->toBeRendered();
        }
    }
};

class SkinnedMesh : public GraphicDataCollector
{
public:
    SkinnedMesh(){}
    Mesh mesh;
    VAO vao;
    std::vector<glm::mat4> bones;
    void toBeRendered(){
            console.clog(__PRETTY_FUNCTION__);
        RenderDataCollector::insert(this);
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
    void toBeRendered(){
            console.clog(__PRETTY_FUNCTION__);
        RenderDataCollector::insert(this);
    }
};
