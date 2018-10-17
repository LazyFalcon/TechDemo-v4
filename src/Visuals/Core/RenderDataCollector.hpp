#pragma once
#include "GPUResources.hpp"
#include "RenderStructs.hpp"
#include <typeindex>
#include <typeinfo>

class SkinnedMesh;
class ArmoredVehicleTracks;
class LightSource;

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
    static std::vector<LightSource*> lights[100]; // * lights collected by type, and relation to camera: index = type + isCameraInsideEnum
    static std::vector<LightSource*> lightsCastingShadows;

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
