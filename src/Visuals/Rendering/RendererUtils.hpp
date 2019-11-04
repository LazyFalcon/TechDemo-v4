#pragma once
#include "Texture.hpp"

class Context;
class Window;
enum class BlurOptions
{
    Symmetrical,
    Anamorphic
};
class RendererUtils
{
    Window& window;
    Context& context;

public:
    RendererUtils(Window& window, Context& context) : window(window), context(context) {}
    void drawBackground(const std::string& image);
    void blurBuffer();
    void renderBlurred();
    void takeScreenShot();
    const Texture& bilateralAOBlur(const Texture& source, float kernel = 5.f);

    Texture extractBrightParts(Texture& source);

    Texture blur12(Texture& source, BlurOptions option = BlurOptions::Symmetrical);
    Texture blur14(Texture& source, BlurOptions option = BlurOptions::Symmetrical);
    Texture blur18(Texture& source, BlurOptions option = BlurOptions::Symmetrical);
};
