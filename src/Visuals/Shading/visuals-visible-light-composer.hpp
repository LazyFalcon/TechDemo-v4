#pragma once
#include "core.hpp"
#include "LightSource.hpp"
#include "visuals-shadow-pool.hpp"

template<typename T>
auto diffContainers(std::vector<T*>& newData, std::vector<T*>& current) {
    std::vector<T*> added, removed;

    std::set_difference(newData.begin(), newData.end(), current.begin(), current.end(), std::inserter(added, added.begin());
    std::set_difference(current.begin(), current.end(), newData.begin(), newData.end(), std::inserter(removed, removed.begin());

    return std::make_tuple(added, removed);
}

class VisibleLightComposer
{
private:
    ShadowPool m_shadowPool;
    using Vec = std::vector<Light*>;
    Vec m_lights;
    Vec m_recentlyRemoved;

    void freeResources(const Vec& toCleanup) {
        for(auto it : toCleanup) {
            m_shadowPool.release(it.assignedTexture, it.assignedTextureSize);
            m_lights.erase(
                std::remove_if(m_lights.begin(), m_lights.end(), [it](auto lightPtr) { return lightPtr == it; }));
            it.assignedTexture = INVALID_TEXTURE;
        }
    }

    uint calcNeededResources(const Vec& added) {
        uint count = 0;
        for(auto it : added) { count += it->assignedTextureSize; }
        return count;
    }
    // void removeLeastImportant(Vec& added, int resourcesToFree) {
    //     // random for now :D
    //     Vec removedSingleTextures;
    //     Vec result;

    //     added.erase(std::remove_if(added.begin(), added.end(), [result, removedSingleTextures](const Light* l) {
    //         if(resourcesToFree > 0) {
    //             resourcesToFree -= l->assignedTextureSize;
    //             if(l->assignedTextureSize == 1)
    //                 removedSingleTextures.intert(l);
    //             return true;
    //         }
    //         else
    //             return false;
    //     })) if(removedSingleTextures.size() > 0 and result < 0) {
    //         added.insert(removedSingleTextures.begin(),
    //                      removedSingleTextures.begin() + std::min(removedSingleTextures.size(), -result));
    //     }
    // }

    Vec selectToCastShadows(Vec& all, std::pair<uint, uint> available) const {
        Vec result;
        for(auto it : all) {
            if(not it->needsShadow)
                continue;

            if(it->assignedTextureSize == 1 and available.first > 0) {
                available.first--;
                result.push_back(it);
            }
            else if(it->assignedTextureSize == 6 and available.second > 0) {
                available.second--;
                result.push_back(it);
            }
            else if(available.first == 0 and available.second == 0)
                break;
        }
        return result;
    }
    Vec allocateResources(const Vec& toAdd) {
        Vec withSucces;
        for(auto it : toAdd) {
            auto result = m_shadowPool.allocate(it->assignedTextureSize);
            if(not result) {
                continue;
            }
            it->assignedTexture = *result;
            it->needsUpdate = true;
            withSucces.insert(it);
        }
        return withSucces;
    }

    Vec whichLightNeedsRefresh(const Vec& lights) const {
        Vec result;
        for(auto it : lights) {
            it->needsUpdate = false;
            result.push_back(it);
        }
    }

    void renderShadows() {
        // obiekty sceny będą z jednej kolejki, co z obiektami dynamicznymi(to samo vao) i pojazdami i drzewami?
        // pojazdy to jest w sumie tylko geometria, więc to da się przygotować
        for(auto it : m_lights) {
            auto toRender = prepareRenderCommandForLight(it);
            if(toRender.skinned) {}
            if(toRender.dummy) {}
            if(toRender.terrain) {}
            if(toRender.foliage) {}
        }
    }

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
    void processVisibleShadows(Vec& lights) {
        std::sort(lights.begin(), lights.end()); // by pointer value
        auto [added, removed] = diffContainers(lights, m_lights);

        // m_recentlyRemoved.insert(removed);

        // no caching for now. In future remove only when there is a need for new resources
        freeResources(removed);
        added = selectToCastShadows(added, m_shadowPool.avaliableResources());
        auto successfullyAllocated = allocateResources(added);

        m_lights.insert(successfullyAllocated.begin(), successfullyAllocated.end(), m_lights.back());

        std::sort(m_lights.begin(), m_lights.end());
        // for now recalculate each light,
        // later check if light moved or achanged any params, of if it has dynamic object in view
        auto needRefresh = whichLightNeedsRerender(m_lights);

        renderShadows(needRefresh);

        // x znaleźć nowe
        // x posortować po odległości/priorytecie(tak dajemy światłom priorytet, jak bardzo wązne są ich cienie)
        // x przeliczyć zapotrzebowanie na tekstury i sprawdzić ilu światłom da się przydzielić
        // przydzielić i przeliczyć macierze projekcji dla świateł(czy to nie powinno być robione w świetle?)

        // kazde światło trzeba przetestować, dynamiczne obiekty sprawdzamy zawsze, to powinno być tanie.
        // statyczne tylko jeśli światło się poruszyło, albo jest nowe
        // jesli ma dynamiczne obiekty lub się poruszyło to trzeba je przerenderować
        // update listy obiektów robi się gdzies indziej, najlepiej przy updatowaniu świateł. -> te które zostały wybrane przez frustum i mają cienie i są odpowiednio blisko są updatowane i

        // na razie bez keszowania(potrzebna jakaś flaga że światło zostało zmodyfikowane): znaleźć światłom obiekty których dotykają, przy pomocy ghost obiektów i bulleta
        // renderujemy każde światło osobno
    }
    void processVisibleLights(Vec& lights) {
        Vec& lightsCastingShadows;
        for(auto it : lights) {
            if(not it->castShadows)
                continue;
        }
    }
};
