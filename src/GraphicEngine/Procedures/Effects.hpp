#pragma once

class Camera;
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
    void drawDecals(Camera &camera);
    void scattering(Scene &scene, Camera &camera);
    void scatteringShadowed(Scene &scene, Camera &camera);
    void sky(Scene &scene, Camera &camera);
    void starfield(Scene &scene, Camera &camera);
    void SSAO(Camera &camera);
    void toneMapping(float);
    void FXAA();
    void chromaticDistortion(glm::vec3 strenght);
    void vignette(float r1, float r2);
    void bloom();
    void bloomSpecular();
    void matcap(Camera &camera);
};
