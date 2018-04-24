#include "UIRender.hpp"
#include "RenderedUI.hpp"
#include "Context.hpp"
#include "Assets.hpp"
#include "PerfTimers.hpp"


void UIRender::depthPrepass(RenderedUI& ui){}

template<>
void UIRender::render(std::vector<RenderedUI::Background>& backgrounds){
    backgrounds.clear();
}

void UIRender::render(RenderedUI& ui){
    // TODO: please rename this functions
    depthPrepass(ui);
    render(ui.get<RenderedUI::Background>());

}

// TODO: it would be good to use stencil here, for drawing UIs
// void UIRender::renderUis(){
//     GPU_SCOPE_TIMER();
//     // renderUIsToTexture(uiUpdater.getUis());
//     composeUIsToScreen();
// }
// void UIRender::renderUIsToTexture(std::vector<std::shared_ptr<UI::IMGUI>> &uis){
//     context.setupFBO_11(context.tex.full.a);

//     gl::ClearColor(0.f, 0.f, 0.f, 0.f);
//     gl::Clear(gl::COLOR_BUFFER_BIT);

//     gl::DepthMask(gl::FALSE_);
//     gl::Disable(gl::DEPTH_TEST);

//     gl::Disable(gl::BLEND);

//     for(auto &ui : uis){
//         // for(auto i=0; i<ui.layers; i++)
//         u32 i = 0;
//         renderBoxes(*ui, i);
//         renderImages(*ui, i);
//     }

//     gl::Enable(gl::BLEND);
//     gl::BlendFunc(gl::ONE, gl::ONE_MINUS_SRC_ALPHA);
//     for(auto &ui : uis){
//         renderFonts(*ui);
//     }

//     gl::BindBuffer(gl::ARRAY_BUFFER, 0);

//     gl::DisableVertexAttribArray(1);
//     gl::DisableVertexAttribArray(2);
//     gl::DisableVertexAttribArray(3);
//     gl::DisableVertexAttribArray(4);
//     gl::Disable(gl::BLEND);
//     context.errors();
// }

// void UIRender::renderBoxes(UI::IMGUI &ui, u32 layer){
//     auto shader = assets::getShader("DrawBox");
//     shader.bind();
//     shader.uniform("uWidth", window.size.x);
//     shader.uniform("uHeight", window.size.y);

//     context.shape.quadCorner.bind().attrib(0).pointer_float(4).divisor(0);
//     // context.getRandomBuffer().update(ui.m_uiGraphic.boxes[layer])
//     //     .attrib(1).pointer_float(4, sizeof(BoxGraphic), (void*)offsetof(BoxGraphic, positionSize)).divisor(1)
//     //     .attrib(2).pointer_color(sizeof(BoxGraphic), (void*)offsetof(BoxGraphic, color)).divisor(1)
//     //     .attrib(3).pointer_color(sizeof(BoxGraphic), (void*)offsetof(BoxGraphic, borderColor)).divisor(1);

//     // gl::DrawArraysInstanced(gl::TRIANGLE_STRIP, 0, 4, ui.m_uiGraphic.boxes[layer].size()); // TODO: check if simple draw arrays wouldn't be faster

//     // ui.m_uiGraphic.boxes[layer].clear();

//     context.errors();
// }

// void UIRender::renderImages(UI::IMGUI &ui, u32 layer){
//     // auto shader = assets::getShader("DrawIcon");
//     // shader.bind();
//     // shader.texture("uTexture", ui.m_imageSet->ID);
//     // shader.uniform("uWidth", window.size.x);
//     // shader.uniform("uHeight", window.size.y);

//     // context.shape.quadCorner.bind().attrib(0).pointer_float(4).divisor(0);

//     // for(auto &it : cnt->m_images.first[layer]){
//     //     shader.uniform("(uColor)", (colorHex(it.color)));
//     //     shader.uniform("uRect", it.rect);
//     //     shader.uniform("uUVs", it.uvs);
//     //     gl::DrawArrays(gl::TRIANGLE_STRIP, 0, 4);
//     // }

//     // context.errors();
// }

// void UIRender::renderFonts(UI::IMGUI &ui){
//     // if(ui.fontRenderer.renderedSymbols.size()){
//     //     auto shader = assets::getShader("UIText");
//     //     shader.bind();
//     //     shader.uniform("uFrameSize", window.size);
//     //     shader.atlas("uTexture", assets::getAlbedoArray("Fonts").id);

//     //     context.shape.quadCorner.bind().attrib(0).pointer_float(4).divisor(0);

//         // context.getRandomBuffer().update(ui.fontRenderer.renderedSymbols)
//         //     .attrib(1).pointer_float(2, sizeof(UI::RenderedSymbol), (void*)offsetof(UI::RenderedSymbol, pxPosition)).divisor(1)
//         //     .attrib(2).pointer_float(2, sizeof(UI::RenderedSymbol), (void*)offsetof(UI::RenderedSymbol, pxSize)).divisor(1)
//         //     .attrib(3).pointer_float(3, sizeof(UI::RenderedSymbol), (void*)offsetof(UI::RenderedSymbol, uv)).divisor(1)
//         //     .attrib(4).pointer_float(2, sizeof(UI::RenderedSymbol), (void*)offsetof(UI::RenderedSymbol, uvSize)).divisor(1)
//         //     .attrib(5).pointer_color(sizeof(UI::RenderedSymbol), (void*)offsetof(UI::RenderedSymbol, color)).divisor(1);

//         // gl::DrawArraysInstanced(gl::TRIANGLE_STRIP, 0, 4, ui.fontRenderer.renderedSymbols.size());
//         // ui.fontRenderer.renderedSymbols.clear();
//     // }

//     context.errors();
// }

// void UIRender::composeUIsToScreen(){
//     context.setupFBO_11(context.tex.gbuffer.color);

//     gl::DepthMask(gl::FALSE_);
//     gl::Disable(gl::DEPTH_TEST);

//     gl::Disable(gl::BLEND);
//     gl::BlendFunc(gl::ONE, gl::ONE_MINUS_SRC_ALPHA);

//     auto shader = assets::getShader("ComposeWithBlurredScene");
//     shader.bind();

//     shader.texture("uBlurred", context.tex.blurredScene, 0);
//     shader.texture("uUIs", context.tex.full.a, 1);
//     context.drawScreen();
// }
