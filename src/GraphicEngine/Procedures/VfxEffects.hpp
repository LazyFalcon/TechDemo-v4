#pragma once

class Camera;
class Context;
class Scene;
class Window;

class VfxEffects
{
private:
    Window &window;
    Context &context;

    void drawLines(Camera &camera);
    void drawPlasmaProjeciles(Camera &camera);
    void drawPlasmaFlashes(Camera &camera);
    void drawVolumetricLaserBeams(Camera &camera);
    void drawSSDots(Camera &camera);
    void drawSparks(Camera &camera);
public:
    VfxEffects(Window &window, Context &context) : window(window), context(context){}

    void drawOpaque(Camera &camera); // directly to gbuffer, without blending and with full features
    void drawGlare(Camera &camera); // directly to gbuffer, without blending and with full features
    void drawTransparent(Camera &camera); // to temporary, with internal OI(?) blending, next with alpha blend to gbuffer, after lights, sometimes casts lights, dont cast shadows,
};
