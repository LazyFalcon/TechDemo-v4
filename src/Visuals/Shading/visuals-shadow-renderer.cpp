#include "core.hpp"
#include "visuals-shadow-renderer.hpp"
#include "Context.hpp"
#include "LightSource.hpp"
#include "visuals-shadow-pool.hpp"

ShadowRenderer::ShadowRenderer(Context& context) : context(context), m_shadowPool(std::make_unique<ShadowPool>) {}

void ShadowRenderer::freeResources(const Vec& toCleanup) {
    for(auto it : toCleanup) {
        m_shadowPool->release(it->shadow.texture, it->shadow.textureCount);
        m_lights.erase(
            std::remove_if(m_lights.begin(), m_lights.end(), [it](auto lightPtr) { return lightPtr == it; }));
        it.shadow.texture = INVALID_TEXTURE;
    }
}
void ShadowRenderer::freeResources(const Vec& toCleanup) {
    for(auto it : toCleanup) {
        m_shadowPool.release(it->shadow.texture, it->shadow.textureCount);
        m_lights.erase(
            std::remove_if(m_lights.begin(), m_lights.end(), [it](auto lightPtr) { return lightPtr == it; }));
        it.shadow.texture = INVALID_TEXTURE;
    }
}

Vec ShadowRenderer::allocateResources(const Vec& toAdd) {
    Vec withSucces;
    for(auto it : toAdd) {
        auto result = m_shadowPool->allocate(it->shadow.textureCount);
        if(not result) {
            continue;
        }
        it->shadow.texture = *result;
        it->needsUpdate = true;
        withSucces.insert(it);
    }
    return withSucces;
}

void ShadowRenderer::processVisibleShadows(Vec& lights) {
    std::sort(lights.begin(), lights.end()); // by pointer value
    auto [added, removed] = diffContainers(lights, m_lights);

    // m_recentlyRemoved.insert(removed);

    // no caching for now. In future remove only when there is a need for new resources
    freeResources(removed);
    added = selectToCastShadows(added, m_shadowPool->avaliableResources());
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
