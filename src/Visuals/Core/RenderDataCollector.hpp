#pragma once
#include "GPUResources.hpp"
#include "RenderStructs.hpp"
#include "Details.hpp"
#include <typeindex>
#include <typeinfo>

class ArmoredVehicleTracks;
class Camera;
class Details;
class GraphicEngine;
class LightSource;
class SkinnedMesh;
class Window;

struct Uniforms
{
    glm::mat4 uView;
    glm::mat4 uInvPV;
    glm::vec4 uEyePosition;
    glm::vec2 uWindowSize;
    glm::vec2 uScreenSize;
    glm::vec2 uPixelSize;
    float uFovTan;
    float uNear;
    float uFar;
    float lastFrameTime;
    float sinceStartTime; // TODO: remember to loop it
    float exposture;
    float gamma;
};

class RenderDataCollector
{
public:
    using tupleOfContainers = std::tuple<
            std::vector<SkinnedMesh*>,
            std::vector<ArmoredVehicleTracks*>,
            std::vector<SimpleModelPbr>,
            std::vector<PointLightSource>
        >;

    static tupleOfContainers collection;
    static CommandArray enviro;
    static CommandArray foliage;
    static Uniforms uniforms;
    static std::vector<LightSource*> lights[100]; // * lights collected by type, and relation to camera: index = type + isCameraInsideEnum
    static std::vector<LightSource*> lightsCastingShadows;

    static void collectCamera(Camera& camera);
    static void collectWindow(Window& window);
    static void collectTime(float lastFrame, u64 sinceStart);

    static GraphicEngine* enginePtr;
    static Details& details();

    template<typename T>
    static std::vector<T>& get(){
        return std::get<std::vector<T>>(collection);
    }
    template<typename T>
    static void insert(const T& t){
        std::get<std::vector<T>>(collection).push_back(t);
    };
    template<typename T>
    static void insert(T* t){
        std::get<std::vector<T*>>(collection).push_back(t);
    };
};
