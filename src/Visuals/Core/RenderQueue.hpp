#pragma once
#include "GPUResources.hpp"
#include "RenderStructs.hpp"
#include <typeindex>
#include <typeinfo>
#include <boost/any.hpp>

// ! when inserts will be too costly, convert this precious thing to tuple of known structures, however pros of 'any' is that now types shouldn't be known
class RenderQueue
{
public:

    static std::map<std::type_index, boost::any> collection;

    template <typename ObjectToRender>
    static void insert(const ObjectToRender &obj){
        auto &anyVec = collection[typeid(ObjectToRender)];

        if(anyVec.empty()) anyVec = std::vector<ObjectToRender>();

        boost::any_cast<std::vector<ObjectToRender>&>(anyVec).push_back(obj);
    }

    template <typename ObjectToRender, typename Tag>
    static void insertTag(const ObjectToRender &obj){
        auto &anyVec = collection[typeid(Tag)];

        if(anyVec.empty()) anyVec = std::vector<ObjectToRender>();

        boost::any_cast<std::vector<ObjectToRender>&>(anyVec).push_back(obj);
    }

    template <typename ObjectToRender>
    static void insert(ObjectToRender* obj){
        auto &anyVec = collection[typeid(ObjectToRender*)];

        if(anyVec.empty()) anyVec = std::vector<ObjectToRender*>();

        boost::any_cast<std::vector<ObjectToRender*>&>(anyVec).push_back(obj);
    }

    template <typename TypeOfObject>
    static std::vector<TypeOfObject> pop(){
        auto &anyVec = collection[typeid(TypeOfObject)];
        if(anyVec.empty()) anyVec = std::vector<TypeOfObject>();

        std::vector<TypeOfObject> tmp;

        tmp.swap(boost::any_cast<std::vector<TypeOfObject>&>(anyVec));

        return tmp;
    }

    template <typename TypeOfObject>
    static std::vector<TypeOfObject>& get(){
        auto &anyVec = collection[typeid(TypeOfObject)];
        if(anyVec.empty()) anyVec = std::vector<TypeOfObject>();

        return boost::any_cast<std::vector<TypeOfObject>&>(anyVec);
    }

    template <typename TypeOfObject, typename Tag>
    static std::vector<TypeOfObject>& get(){
        auto &anyVec = collection[typeid(Tag)];
        if(anyVec.empty()) anyVec = std::vector<TypeOfObject>();

        return boost::any_cast<std::vector<TypeOfObject>&>(anyVec);
    }
};
