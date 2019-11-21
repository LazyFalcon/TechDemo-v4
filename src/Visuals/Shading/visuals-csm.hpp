#pragma once

namespace camera
{
class Camera;
union FrustmCorners;
}
class Context;
class Window;

namespace visuals
{
class MainLightParams;
class CascadedShadowMapping
{
private:
    const Window& window;
    Context& context;
    const int m_numberOfSlices {4};

    void init();
    void calculateShadowProjectionMatrices(const camera::FrustmCorners& slices, const MainLightParams& light);
    void cleanup();

    void renderNonPlayableObjects();
    void renderTerrainFromHeightmap();
    void renderBigFoliage();
    void renderObjectsFromFrustum();
    void renderObjectsOutsideFrustum();

public:
    CascadedShadowMapping(Window& window, Context& context) : window(window), context(context) {}
    void prepare(const MainLightParams& light, camera::Camera& camera);
    void render();
};

}