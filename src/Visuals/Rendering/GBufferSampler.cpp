#include "core.hpp"
#include "GBufferSampler.hpp"
#include "Context.hpp"
#include "PerfTimers.hpp"
#include "Texture.hpp"
#include "Window.hpp"
#include "camera-data.hpp"

std::list<GBufferSampler*> GBufferSamplers::samplers;
void GBufferSamplers::registerMe(GBufferSampler* data) {
    samplers.push_back(data);
}
void GBufferSamplers::deregisterMe(GBufferSampler* data) {
    samplers.remove(data);
}

void GBufferSamplers::sampleGBuffer(camera::Camera& camera) {
    GPU_SCOPE_TIMER();
    gl::Disable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);
    gl::Disable(gl::BLEND);
    // position and depth
    gl::BindFramebuffer(gl::READ_FRAMEBUFFER, context.fbo.full);
    gl::FramebufferTexture2D(gl::READ_FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::TEXTURE_2D, context.tex.gbuffer.depth.ID,
                             0);
    gl::DrawBuffers(0, &context.fbo.drawBuffers[0]);
    for(auto it : samplers) {
        auto screenPosition = it->relativePosition ? window.size * it->samplePosition : it->samplePosition;
        gl::ReadPixels(screenPosition.x, screenPosition.y, 1, 1, gl::DEPTH_COMPONENT, gl::FLOAT, &(it->depth));

        glm::vec4 viewSpace(screenPosition.x / window.size.x * 2.f - 1.f, screenPosition.y / window.size.y * 2.f - 1.f,
                            2 * it->depth - 1, 1);
        glm::vec4 worldPos = camera.invPV * viewSpace;
        worldPos /= worldPos.w;
        worldPos.w = 1;

        it->position = worldPos;
        it->depth = camera.convertDepthToWorld(it->depth);
    }
    // normal
    gl::FramebufferTexture2D(gl::READ_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D,
                             context.tex.gbuffer.normals.ID, 0);
    for(auto it : samplers) {
        auto screenPosition = it->relativePosition ? window.size * it->samplePosition : it->samplePosition;
        uint64_t normal;
        gl::ReadPixels(screenPosition.x, screenPosition.y, 1, 1, gl::RGBA, gl::HALF_FLOAT, &normal);
        it->normal = glm::unpackHalf4x16(normal);
        it->normal.w = 0;
    }
    gl::FramebufferTexture2D(gl::READ_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, 0, 0);

    context.errors();
}
