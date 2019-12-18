#pragma once
#include "LightSource.hpp"

class Context;
class ShadowPool;

class ShadowRenderer
{
private:
    Context& context;
    std::unique_ptr<ShadowPool> m_shadowPool;
    using Vec = std::vector<LightSource*>;
    Vec m_lights;
    Vec m_recentlyRemoved;

    ShadowRenderer(Context& context);

    template<typename T>
    auto diffContainers(std::vector<T*>& newData, std::vector<T*>& current) const {
        std::vector<T*> added, removed;

        std::set_difference(newData.begin(), newData.end(), current.begin(), current.end(),
                            std::inserter(added, added.begin()));
        std::set_difference(current.begin(), current.end(), newData.begin(), newData.end(),
                            std::inserter(removed, removed.begin()));

        return std::make_tuple(added, removed);
    }

    void freeResources(const Vec& toCleanup);

    uint calcNeededResources(const Vec& added) {
        uint count = 0;
        for(auto it : added) { count += it->shadow.textureCount; }
        return count;
    }
    Vec allocateResources(const Vec& toAdd);

    Vec whichLightNeedsRefresh(const Vec& lights) const {
        Vec result;
        for(auto it : lights) {
            it->needsUpdate = false;
            result.push_back(it);
        }
    }

    void renderShadows();

    struct RenderCommand
    {
        RenderCommand() : id(s_id++) {}
        static uint s_id;
        uint id;
        std::optional<> skinned;
        std::optional<> dummy;
        std::optional<> terrain;
        std::optional<> foliage;
    };

    auto prepareRenderCommandForLight(Light* light) {
        RenderCommand renderCommand; // here goes dummy objects, vehicles and
        for(auto it : objectsCastingShadows) { it->addItselfToRenderCommand(renderCommand); }
        return renderCommand;
    }

public:
    void processVisibleShadows(Vec& lights);
};
