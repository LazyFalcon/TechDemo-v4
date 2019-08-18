#pragma once

class Camera::Camera;
class Context;
class RendererUtils;
class Scene;
class Window;

class Effects
{
private:
    Window &window;
    Context &context;
    RendererUtils &utils;
public:
    Effects(Window &window, Context &context, RendererUtils &utils) : window(window), context(context), utils(utils){}
    void drawDecals(Camera::Camera &camera);
    void scattering(Scene &scene, Camera::Camera &camera);
    void scatteringShadowed(Scene &scene, Camera::Camera &camera);
    void sky(Scene &scene, Camera::Camera &camera);
    void starfield(Scene &scene, Camera::Camera &camera);
    void SSAO(Camera::Camera &camera);
    void toneMapping();
    void FXAA();
    void chromaticDistortion(glm::vec3 strenght);
    void vignette(float r1, float r2);
    void bloom();
    void bloomSpecular();
    void matcap(Camera::Camera &camera);
    void filmGrain();
};
