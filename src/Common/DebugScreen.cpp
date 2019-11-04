#include "core.hpp"
#include "DebugScreen.hpp"
#include "Colors.hpp"

// #include "Scene.hpp"
#include "ui.hpp"

namespace Widget
{
struct Option
{
    Option(u64& attrib, u64 value, const std::string& name) : attrib(attrib), value(value), name(name) {}
    u64& attrib;
    u64 value;
    std::string name;
};

void dropDownSelectList(glm::vec2 position, const std::vector<Option>& options, const std::string name, bool& enabled) {
    // auto &ui = Global::main.ui;
    // ui.table(UI::Vertical | UI::ToLeft | UI::ToTop )
    //     .overridePosition(position.x, position.y);

    //     ui.rect(150,20).color(color::white).text((enabled ? "- " : "+ ") + name, font::Default).button(enabled)();

    //     if(enabled){
    //         for(auto &option : options){
    //             bool selected = option.attrib & option.value;
    //             ui.rect(150, 20).color(selected ? color::gold : color::white).text(option.name, font::Default).onLMB([&option]{
    //                     option.attrib ^= option.value;
    //                 })();
    //         }
    //     }

    // ui.endTable();
}

std::vector<Option> renderOptions;

}

std::map<void*, std::function<void(UI::IMGUI&)>> DebugScreen::panels;
void DebugScreen::addPanel(void* user, std::function<void(UI::IMGUI&)> fun) {
    panels[user] = fun;
}
void DebugScreen::removePanel(void* user) {
    panels.erase(user);
}

void DebugScreen::init() {
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::FXAA_, "FXAA");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::WIREFRAME, "Wireframe");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::SCATTERING, "Scattering");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::HDR, "HDR");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::SSAO_, "SSAO");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::SOBEL, "SOBEL");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::DOF, "DOF");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::CHROMATIC_DISTORTION, "Chromatic distortion");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::CHROMATIC_DISTORTIONS, "Chromatic distortions");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::VIGNETTE, "Vignette");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::BLOOM, "Bloom");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::BOKEH_BLUR, "Bokeh Bloom");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::GOD_RAYS, "God rays");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::SHADOWS, "Shadows");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::DRAW_NORMALBUFFER, "Draw normalbuffer");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::DRAW_DEPTHBUFFER, "Draw depthbuffer");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::DRAW_SHADOWBUFFER, "Draw shadowmap");
    // Widget::renderOptions.emplace_back(Global::main.graphicOptions, graphic::DRAW_DEBUG, "Draw debug");
    // Widget::renderOptions.emplace_back(enable2DMap, 1, "Draw 2D map");
    // Widget::renderOptions.emplace_back(enable3DMap, 1, "Draw 3D map");
}

void DebugScreen::show(UI::IMGUI& ui, bool ctrlPressed) {
    // if(ctrlPressed){
    // ui.table(UI::Vertical | UI::ToLeft | UI::ToBottom ).pos(5, 30);

    //     for(auto &fun : panels){
    //         fun.second(ui);
    //     }

    // ui.endTable();
    // }
}

void DebugScreen::options(UI::IMGUI& ui) {
    // Widget::dropDownSelectList(glm::vec2(2, ui.m_maxVertical-22), Widget::renderOptions, "Render options", enableOptions);
}
