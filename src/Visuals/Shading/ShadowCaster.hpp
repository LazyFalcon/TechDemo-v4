#pragma once

namespace camera
{
class Camera;
union FrustmCorners;
}
class Context;
class Scene;
class Window;
class Sun;
class SceneGraph;
class Mesh;

class ShadowCaster
{
private:
    Window& window;
    Context& context;
    int numberOfFrustumSplits {4};

    void initShadowMapCascade();
    std::vector<Mesh> getTerrainToRender(SceneGraph& sg);
    glm::mat4 fitShadowProjectionAroundBoundingBox(camera::FrustmCorners& corners,
                                                   Sun& sun,
                                                   camera::Camera& camera,
                                                   float minZ = 50.f,
                                                   float maxZ = 50.f);
    void calculateShadowProjectionMatrices(std::vector<camera::FrustmCorners>& frustumSlices,
                                           glm::vec4 light,
                                           Sun& sun,
                                           camera::Camera& camera);

public:
    ShadowCaster(Window& window, Context& context) : window(window), context(context) {}
    void prepareForDirectionalShadows(Scene& scene, camera::Camera& camera);
    void finishForDirectionalShadows();
    void renderScene(Scene& scene, camera::Camera& camera);
    void renderTerrain(Scene& scene, camera::Camera& camera);
    void updateShadows();
};
