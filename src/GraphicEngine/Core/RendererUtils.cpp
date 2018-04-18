#include "Context.hpp"
#include "RendererUtils.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "ImageLoader.hpp"
#include "PerfTimers.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

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
    context.setupFBO_12(context.tex.half.a);

    auto shader = assets::getShader("BlurHorizontal");
    shader.bind();
    shader.uniform("uPixelSize", window.pixelSize/2.f);
    shader.texture("uTexture", context.tex.gbuffer.color, 0);
    context.drawScreen();

    // second pass
    context.setupFBO_12(context.tex.half.b);

    shader = assets::getShader("BlurVertical");
    shader.bind();
    shader.uniform("uPixelSize", window.pixelSize/2.f);
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
    context.setupFBO_12(context.tex.half.rg16a);
    {
        auto shader = assets::getShader("BilateralBlurVertical");
        shader.bind();

        shader.uniform("uBlurSize", kernel);
        shader.uniform("uPixelSize", window.pixelSize);
        shader.texture("uTexture", source.ID, 0);

        context.drawScreen();
    }
    gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, context.tex.half.rg16b.ID, 0);
    {
        auto shader = assets::getShader("BilateralBlurHorizontal");
        shader.bind();

        shader.uniform("uBlurSize", kernel);
        shader.uniform("uPixelSize", window.pixelSize);
        shader.texture("uTexture", context.tex.half.rg16a, 0);

        context.drawScreen();
    }

    context.errors();

    return context.tex.half.rg16b;
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

    if(not ImageUtils::saveFromMemory("../screenshots/shot_"+boost::posix_time::to_iso_string(time)+".jpg", ImageDataType::RGB8, image)){
        error("Unable to save screenshot");
    }
}


Texture RendererUtils::extractBrightParts(Texture& source){
    context.setupFBO_11(context.tex.full.a);

    auto shader = assets::getShader("BrightPartsExtraction").bind();
    // shader.uniform("uThreshold", 0.9999f);
    shader.texture("uTexture", source, 0);

    context.drawScreen();

    context.errors();

    return context.tex.full.a;
}
Texture RendererUtils::blur12(Texture& source, BlurOptions option){
    if(option == BlurOptions::Symmetrical){
        context.setupFBO_12(context.tex.half.a);

        auto shader = assets::getShader("BlurVertical").bind();
        shader.uniform("uPixelSize", window.pixelSize*2.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.half.b);

        shader = assets::getShader("BlurHorizontal").bind();
        shader.uniform("uPixelSize", window.pixelSize*2.f);
        shader.texture("uTexture", context.tex.half.a, 0);
        context.drawScreen();
    }
    if(option == BlurOptions::Anamorphic){
        context.setupFBO_12(context.tex.half.a);

        auto shader = assets::getShader("BlurVertical_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*2.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.half.b);

        shader = assets::getShader("BlurHorizontal_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*2.f);
        shader.texture("uTexture", context.tex.half.a, 0);
        context.drawScreen();
    }

    context.errors();

    return context.tex.half.b;
}
Texture RendererUtils::blur14(Texture& source, BlurOptions option){
    if(option == BlurOptions::Symmetrical){
        context.setupFBO_14(context.tex.quarter.a);

        auto shader = assets::getShader("BlurVertical").bind();
        shader.uniform("uPixelSize", window.pixelSize*4.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.quarter.b);

        shader = assets::getShader("BlurHorizontal").bind();
        shader.uniform("uPixelSize", window.pixelSize*4.f);
        shader.texture("uTexture", context.tex.quarter.a, 0);
        context.drawScreen();
    }
    if(option == BlurOptions::Anamorphic){
        context.setupFBO_14(context.tex.quarter.a);

        auto shader = assets::getShader("BlurVertical_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*4.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.quarter.b);

        shader = assets::getShader("BlurHorizontal_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*4.f);
        shader.texture("uTexture", context.tex.quarter.a, 0);
        context.drawScreen();
    }

    context.errors();

    return context.tex.quarter.b;
}
Texture RendererUtils::blur18(Texture& source, BlurOptions option){
    if(option == BlurOptions::Symmetrical){
        context.setupFBO_18(context.tex.eight.a);

        auto shader = assets::getShader("BlurVertical").bind();
        shader.uniform("uPixelSize", window.pixelSize*8.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.eight.b);

        shader = assets::getShader("BlurHorizontal").bind();
        shader.uniform("uPixelSize", window.pixelSize*8.f);
        shader.texture("uTexture", context.tex.eight.a, 0);
        context.drawScreen();
    }
    if(option == BlurOptions::Anamorphic){
        context.setupFBO_18(context.tex.eight.a);

        auto shader = assets::getShader("BlurVertical_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*8.f);
        shader.texture("uTexture", source, 0);
        context.drawScreen();


        context.bindTexture(context.tex.eight.b);

        shader = assets::getShader("BlurHorizontal_Anamorphic").bind();
        shader.uniform("uPixelSize", window.pixelSize*8.f);
        shader.texture("uTexture", context.tex.eight.a, 0);
        context.drawScreen();
    }

    context.errors();

    return context.tex.eight.b;
}
