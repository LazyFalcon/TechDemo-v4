#include "ui-renderer.hpp"
#include "ui-rendered.hpp"
#include "ui-text.hpp"
#include "Context.hpp"
#include "Assets.hpp"
#include "PerfTimers.hpp"


void UIRender::blurBackgroundCumulative(RenderedUIItems&){}
void UIRender::blurBackgroundEven(RenderedUIItems& ui){
    auto& polygons = ui.get<RenderedUIItems::ToBlur>();
    m_context.shape.quadCorner.bind().attrib(0).pointer_float(4).divisor(0);

    // first pass

    m_context.fbo[2].tex(m_context.tex.half.a)();

    auto shader = assets::bindShader("blur-horizontal");
    shader.uniform("pxViewSize", m_window.size*0.5f);
    shader.uniform("uTexelSize", m_window.pixelSize*2.f);
    shader.texture("uTexture", m_context.tex.gbuffer.color, 0);
    for(auto& poly : polygons){
        shader.uniform("pxBlurPolygon", poly.poly*0.5f);
        gl::DrawArrays(gl::TRIANGLE_STRIP, 0, 4);
    }

    m_context.fbo[2].tex(m_context.tex.half.b)();

    shader = assets::bindShader("blur-vertical");
    shader.uniform("pxViewSize", m_window.size*0.5f);
    shader.uniform("uTexelSize", m_window.pixelSize*2.f);
    shader.texture("uTexture", m_context.tex.half.a, 0);
    for(auto& poly : polygons){
        shader.uniform("pxBlurPolygon", poly.poly*0.5f);
        gl::DrawArrays(gl::TRIANGLE_STRIP, 0, 4);
    }


    shader = assets::bindShader("copy-rect");
    m_context.fbo[1].tex(m_context.tex.full.a)();

    shader.uniform("pxViewSize", m_window.size);
    shader.texture("uTexture", m_context.tex.half.b, 0);
    for(auto& poly : polygons){
        shader.uniform("pxCopyPolygon", poly.poly);
        gl::DrawArrays(gl::TRIANGLE_STRIP, 0, 4);
    }

    polygons.clear();
    m_context.errors();
}

void UIRender::depthPrepass(RenderedUIItems& ui){

    m_context.fbo[1].tex(m_context.tex.gbuffer.depth)();
    gl::ClearDepth(1);
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    gl::DepthMask(gl::TRUE_);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthFunc(gl::LEQUAL);
    gl::DepthRange(0.0f, 1.0f);
    gl::Disable(gl::BLEND);

    auto& backgrounds = ui.get<RenderedUIItems::Background>();
    std::sort(backgrounds.begin(), backgrounds.end(), [](const auto& a, const auto& b){return a.depth < b.depth;});

    auto shader = assets::getShader("ui-panel-background-depth");
    shader.bind();
    shader.uniform("uWidth", m_window.size.x);
    shader.uniform("uHeight", m_window.size.y);

    m_context.shape.quadCorner.bind().attrib(0).pointer_float(4).divisor(0);
    m_context.getRandomBuffer().update(backgrounds)
        .attrib(1).pointer_float(4, sizeof(RenderedUIItems::Background), (void*)offsetof(RenderedUIItems::Background, box)).divisor(1)
        .attrib(2).pointer_float(1, sizeof(RenderedUIItems::Background), (void*)offsetof(RenderedUIItems::Background, texture)).divisor(1)
        .attrib(3).pointer_float(1, sizeof(RenderedUIItems::Background), (void*)offsetof(RenderedUIItems::Background, depth)).divisor(1)
        .attrib(4).pointer_color(sizeof(RenderedUIItems::Background), (void*)offsetof(RenderedUIItems::Background, color)).divisor(1);

    gl::DrawArraysInstanced(gl::TRIANGLE_STRIP, 0, 4, backgrounds.size());
    m_context.errors();
}

template<>
void UIRender::render(std::vector<RenderedUIItems::Background>& backgrounds){
    auto shader = assets::getShader("ui-panel-background");
    shader.bind();
    shader.uniform("uWidth", m_window.size.x);
    shader.uniform("uHeight", m_window.size.y);

    gl::DrawArraysInstanced(gl::TRIANGLE_STRIP, 0, 4, backgrounds.size());
    m_context.errors();

    gl::BindBuffer(gl::ARRAY_BUFFER, 0);

    gl::DisableVertexAttribArray(1);
    gl::DisableVertexAttribArray(2);
    gl::DisableVertexAttribArray(3);
    gl::DisableVertexAttribArray(4);

    backgrounds.clear();
}

template<>
void UIRender::render(std::vector<RenderedUIItems::ColoredBox>& coloredBoxes){
    auto shader = assets::getShader("ui-panel-coloredPolygon");
    shader.bind();
    shader.uniform("uWidth", m_window.size.x);
    shader.uniform("uHeight", m_window.size.y);

    m_context.shape.quadCorner.bind().attrib(0).pointer_float(4).divisor(0);
    m_context.getRandomBuffer().update(coloredBoxes)
        .attrib(1).pointer_float(4, sizeof(RenderedUIItems::ColoredBox), (void*)offsetof(RenderedUIItems::ColoredBox, box)).divisor(1)
        .attrib(2).pointer_float(1, sizeof(RenderedUIItems::ColoredBox), (void*)offsetof(RenderedUIItems::ColoredBox, depth)).divisor(1)
        .attrib(3).pointer_color(sizeof(RenderedUIItems::ColoredBox), (void*)offsetof(RenderedUIItems::ColoredBox, color)).divisor(1);

    gl::DrawArraysInstanced(gl::TRIANGLE_STRIP, 0, 4, coloredBoxes.size());
    m_context.errors();

    gl::BindBuffer(gl::ARRAY_BUFFER, 0);

    gl::DisableVertexAttribArray(1);
    gl::DisableVertexAttribArray(2);
    gl::DisableVertexAttribArray(3);
    gl::DisableVertexAttribArray(4);

    coloredBoxes.clear();
}

template<>
void UIRender::render(std::vector<Text::Rendered>& text){
    auto shader = assets::getShader("ui-text");
    shader.bind();
    shader.uniform("uFrameSize", m_window.size);

    shader.atlas("uTexture", assets::getAtlas("Fonts").id);

    m_context.shape.quadCorner.bind().attrib(0).pointer_float(4).divisor(0);
    m_context.getRandomBuffer().update(text)
        .attrib(1).pointer_float(4, sizeof(Text::Rendered), (void*)offsetof(Text::Rendered, polygon)).divisor(1)
        .attrib(2).pointer_float(3, sizeof(Text::Rendered), (void*)offsetof(Text::Rendered, uv)).divisor(1)
        .attrib(3).pointer_float(2, sizeof(Text::Rendered), (void*)offsetof(Text::Rendered, uvSize)).divisor(1)
        .attrib(4).pointer_float(1, sizeof(Text::Rendered), (void*)offsetof(Text::Rendered, depth)).divisor(1)
        .attrib(5).pointer_color(sizeof(Text::Rendered), (void*)offsetof(Text::Rendered, color)).divisor(1);

    gl::DrawArraysInstanced(gl::TRIANGLE_STRIP, 0, 4, text.size()); // TODO: check if simple draw arrays wouldn't be faster
    m_context.errors();

    gl::BindBuffer(gl::ARRAY_BUFFER, 0);

    gl::DisableVertexAttribArray(1);
    gl::DisableVertexAttribArray(2);
    gl::DisableVertexAttribArray(3);
    gl::DisableVertexAttribArray(4);
    gl::DisableVertexAttribArray(5);

    text.clear();
}

void UIRender::render(RenderedUIItems& ui){
    // auto fullA = m_context.tex.full.a.acquire();
    // auto depth = m_context.tex.gbuffer.depth.acquire();
    m_context.defaultVAO.bind();
    // for all renderable objects(polygons) render depth
    depthPrepass(ui);

    gl::DepthMask(gl::FALSE_);
    gl::Disable(gl::DEPTH_TEST);

    m_context.fbo[1].tex(m_context.tex.full.a).tex(m_context.tex.gbuffer.depth)();

    gl::ClearColor(0.f, 0.f, 0.f, 0.f);
    gl::Clear(gl::COLOR_BUFFER_BIT);

    blurBackgroundEven(ui);

    m_context.fbo[1].tex(m_context.tex.full.a).tex(m_context.tex.gbuffer.depth)();

    gl::Enable(gl::BLEND);
    gl::BlendFunc(gl::ONE, gl::ONE_MINUS_SRC_ALPHA);

    // render panel backgrounds, sorted, without depth test
    render(ui.get<RenderedUIItems::Background>());

    gl::Enable(gl::DEPTH_TEST);
    // render as-is with depth enabled
    render(ui.get<RenderedUIItems::ColoredBox>());
    // render the rest with depth test
    render(ui.get<Text::Rendered>());


    m_context.fbo[1].tex(m_context.tex.gbuffer.color)();

    gl::Disable(gl::DEPTH_TEST);
    gl::Enable(gl::BLEND);
    gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);

    auto shader = assets::getShader("ApplyFBO");
    shader.bind();
    shader.texture("uTexture", m_context.tex.full.a);
    m_context.drawScreen();

}
