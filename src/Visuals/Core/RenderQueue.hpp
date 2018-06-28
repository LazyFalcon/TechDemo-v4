#pragma once
#include "GPUResources.hpp"
#include "RenderStructs.hpp"
#include <typeindex>
#include <typeinfo>

class SkinnedMesh;
class ArmoredVehicleTracks;

class RenderQueue
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

    template<typename T>
    static std::vector<T>& get(){
        return std::get<std::vector<T>>(collection);
    }
    template<typename T>
    static void insert(const T& t){
        std::get<std::vector<T>>(collection).push_back(t);
    } ;
    template<typename T>
    static void insert(T* t){
        std::get<std::vector<T*>>(collection).push_back(t);
    } ;
};
