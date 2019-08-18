#pragma once

class Camera::Camera;
class Context;
class Scene;
class Window;
union FrustmCorners;
class Sun;
class SceneGraph;
class Mesh;

class ShadowCaster
{
private:
    Window &window;
    Context &context;
    int numberOfFrustumSplits {4};

    void initShadowMapCascade();
    std::vector<Mesh> getTerrainToRender(SceneGraph &sg);
    glm::mat4 fitShadowProjectionAroundBoundingBox(FrustmCorners &corners, Sun &sun, Camera::Camera &camera, float minZ=50.f, float maxZ=50.f);
    void calculateShadowProjectionMatrices(std::vector<FrustmCorners> &frustumSlices, glm::vec4 light, Sun &sun, Camera::Camera &camera);
public:
    ShadowCaster(Window &window, Context &context) : window(window), context(context){}
    void prepareForDirectionalShadows(Scene &scene, Camera::Camera &camera);
    void finishForDirectionalShadows();
    void renderScene(Scene &scene, Camera::Camera &camera);
    void renderTerrain(Scene &scene, Camera::Camera &camera);
    void updateShadows();
};
