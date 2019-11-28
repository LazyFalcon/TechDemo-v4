#include "core.hpp"
#include "gl_core_4_5.hpp"
#include "FBO.hpp"
#include "Context.hpp"
#include "Logger.hpp"
#include "Texture.hpp"

FBO& FboHolder::operator[](int i) {
    if(i != m_currentFboId and m_fbos[i].id != 0) {
        m_currentFboId = i;
        gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, m_fbos[i].id);

        debug::logFboChange(m_fbos[i].id);
        // console.log("changed", "i:", i, ", m_currentFboId:", m_currentFboId, ", m_fbos[i].id:", m_fbos[i].id);
    }
    else {
        // console.log("i:", i, ", m_currentFboId:", m_currentFboId, ", m_fbos[i].id:", m_fbos[i].id);
    }
    m_current = &m_fbos[i];
    return m_fbos[i];
}

void FboHolder::unbind() {
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    m_currentFboId = FBO_TYPES_END;
    m_current = nullptr;
}
void FBO::viewport(int x, int y, int z, int w) {
    m_viewport = {x, y, z, w};
}

FBO& FBO::tex(Texture& texture) {
    if(texture.format == gl::DEPTH_COMPONENT) {
        debug::logTargetChange(texture.ID, gl::DEPTH_ATTACHMENT);
        gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::TEXTURE_2D, texture.ID, 0);
    }
    else {
        debug::logTargetChange(texture.ID, gl::COLOR_ATTACHMENT0 + m_attachedTextures);
        gl::FramebufferTexture(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0 + m_attachedTextures++, texture.ID, 0);
    }

    return *this;
}

FBO& FBO::tex(Texture& texture, int id) {
    if(texture.format == gl::DEPTH_COMPONENT) {
        debug::logTargetChange(texture.ID, gl::DEPTH_ATTACHMENT);
        gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::TEXTURE_2D, texture.ID, 0);
    }
    else {
        debug::logTargetChange(texture.ID, gl::COLOR_ATTACHMENT0 + id);
        gl::FramebufferTexture(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0 + id, texture.ID, 0);
    }

    return *this;
}
FBO& FBO::tex(int texture, int id) {
    debug::logTargetChange(texture, gl::COLOR_ATTACHMENT0 + id);
    gl::FramebufferTexture(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0 + id, texture, 0);

    return *this;
}

FBO& FBO::operator()() {
    gl::DrawBuffers(m_attachedTextures, state->drawBuffers);
    gl::Viewport(m_viewport.x, m_viewport.y, m_viewport.z, m_viewport.w);

    if(!hasColor) {
        gl::ColorMask(gl::FALSE_, gl::FALSE_, gl::FALSE_, gl::FALSE_);
        state->colorDisabled = true;
    }
    if(hasColor and state->colorDisabled) {
        gl::ColorMask(gl::TRUE_, gl::TRUE_, gl::TRUE_, gl::TRUE_);
        state->colorDisabled = false;
    }

    m_attachedTextures = 0;
    return *this;
}
