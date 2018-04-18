#pragma once
#include "Includes.hpp"

class Context;
class Window;
namespace UI {class IMGUI; class Updater;}

class UIDrawer
{
    Window &window;
    Context &context;
    UI::Updater &uiUpdater;
    void renderUIsToTexture(std::vector<std::shared_ptr<UI::IMGUI>> &uis);
    void renderBoxes(UI::IMGUI &ui, u32 layer);
    void renderImages(UI::IMGUI &ui, u32 layer);
    void renderFonts(UI::IMGUI &ui);
    void composeUIsToScreen();
public:
    UIDrawer(Window &window, Context &context, UI::Updater &uiUpdater) : window(window), context(context), uiUpdater(uiUpdater){}
    void renderUis();
};
