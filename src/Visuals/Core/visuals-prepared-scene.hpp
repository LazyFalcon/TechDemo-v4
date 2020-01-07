#pragma once
#include <typeindex>
#include <typeinfo>
#include "GPUResources.hpp"
#include "LightSource.hpp"
#include "RenderStructs.hpp"
#include "camera-data.hpp"

class ArmoredVehicleTracks;
namespace camera
{
class Camera;
}
class Details;
class GraphicEngine;
class LightSource;
class SkinnedMesh;
class Window;

namespace visuals
{
struct Uniforms
{
    glm::mat4 uView;
    glm::mat4 uPV;
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

struct MainLightParams
{
    glm::vec4 direction;
    glm::vec4 color;
    float colorTemperature;
    float power;
};

class PreparedScene
{
public:
    using tupleOfContainers = std::tuple<std::vector<SkinnedMesh*>, std::vector<ArmoredVehicleTracks*>,
                                         std::vector<SimpleModelPbr>, std::vector<PointLightSource>>;

    tupleOfContainers collection;
    CommandArray nonPlayableInsideFrustum;
    CommandArray nonPlayableOutsideFrustum;
    CommandArray foliage;
    Uniforms uniforms;
    std::vector<LightSource*>
        lights[10]; // * lights collected by type, and relation to camera: index = type + isCameraInsideEnum
    std::vector<LightSource*> lightsCastingShadows;

    MainLightParams mainLight;

    camera::Camera cameraOfThisFrame;

    void collectCamera(camera::Camera& camera);
    void collectWindow(Window& window);
    void collectTime(float lastFrame, u64 sinceStart);

    template<typename T>
    std::vector<T>& get() {
        return std::get<std::vector<T>>(collection);
    }
    template<typename T>
    void insert(const T& t) {
        std::get<std::vector<T>>(collection).push_back(t);
    };
    template<typename T>
    void insert(T* t) {
        std::get<std::vector<T*>>(collection).push_back(t);
    };

    void insert(LightSource* t) {
        lights[0].push_back(t);
        lights[t->m_type + t->m_cameraInside].push_back(t);
        if(t->isCastingShadows)
            lightsCastingShadows.push_back(t);
    };

    void clear();
};

extern PreparedScene preparedScene;
}
