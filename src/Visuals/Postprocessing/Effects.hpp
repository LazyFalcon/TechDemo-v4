#pragma once

namespace camera
{
class Camera;
}
class Context;
class RendererUtils;
class Scene;
class Window;

class Effects
{
private:
    Window& window;
    Context& context;
    RendererUtils& utils;

public:
    Effects(Window& window, Context& context, RendererUtils& utils) : window(window), context(context), utils(utils) {}
    void drawDecals(camera::Camera& camera);
    void scattering(Scene& scene, camera::Camera& camera);
    void scatteringShadowed(Scene& scene, camera::Camera& camera);
    void sky(Scene& scene, camera::Camera& camera);
    void starfield(Scene& scene, camera::Camera& camera);
    void SSAO(camera::Camera& camera);
    void toneMapping();
    void FXAA();
    void chromaticDistortion(glm::vec3 strenght);
    void vignette(float r1, float r2);
    void bloom();
    void bloomSpecular();
    void matcap(camera::Camera& camera);
    void filmGrain();
};
