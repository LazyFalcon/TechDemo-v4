#pragma once

class Camera::Camera;
class Context;
class Scene;
class Window;

class VfxEffects
{
private:
    Window &window;
    Context &context;

    void drawLines(Camera::Camera &camera);
    void drawPlasmaProjeciles(Camera::Camera &camera);
    void drawPlasmaFlashes(Camera::Camera &camera);
    void drawVolumetricLaserBeams(Camera::Camera &camera);
    void drawSSDots(Camera::Camera &camera);
    void drawSparks(Camera::Camera &camera);
public:
    VfxEffects(Window &window, Context &context) : window(window), context(context){}

    void drawOpaque(Camera::Camera &camera); // directly to gbuffer, without blending and with full features
    void drawGlare(Camera::Camera &camera); // directly to gbuffer, without blending and with full features
    void drawTransparent(Camera::Camera &camera); // to temporary, with internal OI(?) blending, next with alpha blend to gbuffer, after lights, sometimes casts lights, dont cast shadows,
};
