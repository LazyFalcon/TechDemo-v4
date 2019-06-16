#include "core.hpp"
#include "Context.hpp"
#include "RendererUtils.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "ImageLoader.hpp"
#include "PerfTimers.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

// ! TODO: remember about scalling and window proportions!!
void RendererUtils::drawBackground(const std::string &image){
    gl::Disable(gl::DEPTH_TEST);
    gl::Disable(gl::BLEND);
    gl::Disable(gl::CULL_FACE);
    gl::DepthMask(gl::FALSE_);

    auto shader = assets::getShader("ApplyFBO");
    shader.bind();
    shader.texture("uTexture", assets::getImage(image).ID, 0);
    context.drawScreen();
}

void RendererUtils::blurBuffer(){
    GPU_SCOPE_TIMER();
    gl::Disable(gl::DEPTH_TEST);
    gl::Disable(gl::BLEND);
    gl::Disable(gl::CULL_FACE);
    gl::DepthMask(gl::FALSE_);

    context.tex.gbuffer.color.genMipmaps();

    // first pass
    context.fbo[2].tex(context.tex.half.a)();

    auto shader = assets::getShader("blur-horizontal");
    shader.bind();
    shader.uniform("pxBlurPolygon", glm::vec4(0,0,window.size/2.f));
    shader.uniform("pxWindowSize", window.size/2.f);
    shader.uniform("uPixelSize", window.pixelSize*2.f);
    shader.texture("uTexture", context.tex.gbuffer.color, 0);
    context.drawScreen();

    // second pass
    context.fbo[2].tex(context.tex.half.b)();

    shader = assets::getShader("blur-vertical");
    shader.bind();
    shader.uniform("pxBlurPolygon", glm::vec4(0,0,window.size/2.f));
    shader.uniform("pxWindowSize", window.size/2.f);
    shader.uniform("uPixelSize", window.pixelSize*2.f);
    shader.texture("uTexture", context.tex.half.a, 0);
    context.drawScreen();

    context.tex.blurredScene = context.tex.half.b;
    // context.setupFBO_11(context.tex.gbuffer.color);
}

void RendererUtils::renderBlurred(){
    auto shader = assets::getShader("ApplyFBO");
    shader.bind();
    shader.texture("uTexture", context.tex.blurredScene, 0);
}

/**
 *  Here we are assuming that AO is rendered to half texture
 *  no blending, everything is set for rendering
 *
 * new doc: http://graphics.cs.williams.edu/papers/SAOHPG12/McGuire12SAO.pdf
 *
 */
const Texture& RendererUtils::bilateralAOBlur(const Texture &source, float kernel){
    context.fbo[1].tex(context.tex.full.rg16b)();
    {
        auto shader = assets::getShader("BilateralBlurVertical");
        shader.bind();

        shader.uniform("uBlurSize", kernel);
        shader.uniform("uPixelSize", window.pixelSize);
        shader.texture("uTexture", source.ID, 0);

        context.drawScreen();
    }
    gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, context.tex.full.rg16a.ID, 0);
    {
        auto shader = assets::getShader("BilateralBlurHorizontal");
        shader.bind();

        shader.uniform("uBlurSize", kernel);
        shader.uniform("uPixelSize", window.pixelSize);
        shader.texture("uTexture", context.tex.full.rg16b, 0);

        context.drawScreen();
    }

    context.errors();

    return context.tex.full.rg16a;
}

void RendererUtils::takeScreenShot(){
    float width = window.size.x;
    float height = window.size.y;
    std::vector<u8> data(width * height * 3);
    gl::ReadBuffer(gl::BACK);
    gl::ReadPixels(0, 0, width, height, gl::BGR, gl::UNSIGNED_BYTE, data.data());

    ImageUtils::ImageParams image {};
    image.width = width;
    image.height = height;
    image.dataType = ImageDataType::RGB8;
    image.data = (void*)data.data();
    image.dataSize = data.size()*sizeof(u8);

    boost::posix_time::ptime time(boost::posix_time::second_clock::local_time());

    if(not ImageUtils::saveFromMemory(screenshotsPath + "shot_"+boost::posix_time::to_iso_string(time), ImageDataType::RGB8, image)){
        console.error("Unable to save screenshot");
    }
}


Texture RendererUtils::extractBrightParts(Texture& source){
    context.fbo[1].tex(context.tex.full.a)();

    auto shader = assets::getShader("BrightPartsExtraction").bind();
    // shader.uniform("uThreshold", 0.9999f);
    shader.texture("uTexture", source, 0);

    context.drawScreen();

    context.errors();

    return context.tex.full.a;
}
Texture RendererUtils::blur12(Texture& source, BlurOptions option){
    if(option == BlurOptions::Symmetrical){
        context.fbo[2].tex(context.tex.half.a)();

        auto shader = assets::getShader("blur-horizontal").bind();
        shader.uniform("uPixelSize", window.pixelSize*2.f);
        shader.uniform("pxBlurPolygon", glm::vec4(0,0,window.size/2.f));
        shader.uniform("pxWindowSize", window.size/2.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.half.b);

        shader = assets::getShader("blur-vertical").bind();
        shader.uniform("uPixelSize", window.pixelSize*2.f);
        shader.uniform("pxBlurPolygon", glm::vec4(0,0,window.size/2.f));
        shader.uniform("pxWindowSize", window.size/2.f);
        shader.texture("uTexture", context.tex.half.a, 0);
        context.drawScreen();
    }
    if(option == BlurOptions::Anamorphic){
        context.fbo[2].tex(context.tex.half.a)();

        auto shader = assets::getShader("blur-vertical_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*2.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.half.b);

        shader = assets::getShader("blur-horizontal_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*2.f);
        shader.texture("uTexture", context.tex.half.a, 0);
        context.drawScreen();
    }

    context.errors();

    return context.tex.half.b;
}
Texture RendererUtils::blur14(Texture& source, BlurOptions option){
    if(option == BlurOptions::Symmetrical){
        context.fbo[4].tex(context.tex.quarter.a)();

        auto shader = assets::getShader("blur-vertical").bind();
        shader.uniform("uPixelSize", window.pixelSize*4.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.quarter.b);

        shader = assets::getShader("blur-horizontal").bind();
        shader.uniform("uPixelSize", window.pixelSize*4.f);
        shader.texture("uTexture", context.tex.quarter.a, 0);
        context.drawScreen();
    }
    if(option == BlurOptions::Anamorphic){
        context.fbo[4].tex(context.tex.quarter.a)();

        auto shader = assets::getShader("blur-vertical_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*4.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.quarter.b);

        shader = assets::getShader("blur-horizontal_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*4.f);
        shader.texture("uTexture", context.tex.quarter.a, 0);
        context.drawScreen();
    }

    context.errors();

    return context.tex.quarter.b;
}
Texture RendererUtils::blur18(Texture& source, BlurOptions option){
    if(option == BlurOptions::Symmetrical){
        context.fbo[8].tex(context.tex.eight.a)();

        auto shader = assets::getShader("blur-vertical").bind();
        shader.uniform("uPixelSize", window.pixelSize*8.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.eight.b);

        shader = assets::getShader("blur-horizontal").bind();
        shader.uniform("uPixelSize", window.pixelSize*8.f);
        shader.texture("uTexture", context.tex.eight.a, 0);
        context.drawScreen();
    }
    if(option == BlurOptions::Anamorphic){
        context.fbo[8].tex(context.tex.eight.a)();

        auto shader = assets::getShader("blur-vertical_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*8.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.eight.b);

        shader = assets::getShader("blur-horizontal_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*8.f);
        shader.texture("uTexture", context.tex.eight.a, 0);
        context.drawScreen();
    }

    context.errors();

    return context.tex.eight.b;
}
