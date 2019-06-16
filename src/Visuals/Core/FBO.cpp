#include "core.hpp"
#include "gl_core_4_5.hpp"
#include "FBO.hpp"
#include "Logger.hpp"
#include "Texture.hpp"
#include "Window.hpp"


FBO& FBO::operator[](int layer){
    switch(layer){
        case FULL: viewport = {1, 1}; return setLayer(full);
        case HALF: viewport = {0.5f, 0.5f}; return setLayer(_12);
        case QUARTER: viewport = {0.25f, 0.25f}; return setLayer(_14);
        case 8: viewport = {0.125f, 0.125f}; return setLayer(_18);
        case HALF_WIDE: viewport = {1, 0.5f}; return setLayer(_12_wide);
        case SHADOWMAP: viewport = {1,1}; return setLayer(shadowmap);

        default: console.error("FBO layer:", layer, "doesn't exists");
    }
    return *this;
}
FBO& FBO::setLayer(unsigned int fbo){
    if(fbo != currentFbo){
        gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo);
        fbo = currentFbo;
    }

    return *this;
}

FBO& FBO::tex(Texture& texture){
    if(texture.format == gl::DEPTH_COMPONENT)
        gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::TEXTURE_2D, texture.ID, 0);

    else
        gl::FramebufferTexture(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0+attachedTextures++, texture.ID, 0);

    return *this;
}

FBO& FBO::operator()(){
    gl::DrawBuffers(attachedTextures, drawBuffers);
    gl::Viewport(0, 0, viewport[0]*m_window.size.x, viewport[1]*m_window.size.y);

    attachedTextures = 0;
    return *this;
}

FBO& FBO::setupDefaults(){
    gl::DepthRange(0.0f, 1.0f);
    gl::DepthFunc(gl::LEQUAL);
    gl::FrontFace(gl::CCW);
    gl::Disable(gl::BLEND);
    gl::DepthMask(gl::TRUE_);
    gl::Disable(gl::CULL_FACE);
    gl::Enable(gl::DEPTH_TEST);
    gl::DisableVertexAttribArray(0);
    gl::BindTexture(gl::TEXTURE_2D, 0);
    return *this;
}
