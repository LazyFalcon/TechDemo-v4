#pragma once

namespace camera
{
class Camera;
}
class Context;
class Scene;
class Window;

class VfxEffects
{
private:
    Window& window;
    Context& context;

    void drawLines(camera::Camera& camera);
    void drawPlasmaProjeciles(camera::Camera& camera);
    void drawPlasmaFlashes(camera::Camera& camera);
    void drawVolumetricLaserBeams(camera::Camera& camera);
    void drawSSDots(camera::Camera& camera);
    void drawSparks(camera::Camera& camera);

public:
    VfxEffects(Window& window, Context& context) : window(window), context(context) {}

    void drawOpaque(camera::Camera& camera); // directly to gbuffer, without blending and with full features
    void drawGlare(camera::Camera& camera);  // directly to gbuffer, without blending and with full features
    void drawTransparent(
        camera::Camera&
            camera); // to temporary, with internal OI(?) blending, next with alpha blend to gbuffer, after lights, sometimes casts lights, dont cast shadows,
};
