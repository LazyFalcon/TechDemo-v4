#include "core.hpp"
#include "Assets.hpp"
#include "Context.hpp"
#include "Environment.hpp"
#include "ImageLoader.hpp"
#include "MapProcessing.hpp"
#include "RendererUtils.hpp"
#include "Scene.hpp"

void updateMinMax(ResultMap& inout){
    float min(10000), max(-10000);
    for(auto&it : inout.heightmap){
        if(it < min) min = it;
        else if(it > max) it = max;
    }

    inout.min = min;
    inout.max = max;
    log("map min/max:", min, max);
}

void pathifinderProcessAndSaveDepthMap(Scene& scene, Context& context, ResultMap& inout){
    int width(inout.width), height(inout.height);
    //* create texture
    auto texture = Texture(gl::TEXTURE_2D, gl::DEPTH_COMPONENT32F, width, height, 1, gl::DEPTH_COMPONENT, gl::FLOAT, gl::LINEAR, 0);

    //* create framebuffer
    uint fbo;
    gl::GenFramebuffers(1, &fbo);
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::TEXTURE_2D, texture.ID, 0);
    gl::Viewport(0, 0, width, height);
    gl::DrawBuffers(0, context.fbo.drawBuffers);
    gl::ClearDepth(1000);
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    gl::Enable(gl::DEPTH_TEST);
    gl::DepthMask(gl::TRUE_);
    gl::Disable(gl::BLEND);

    gl::DepthFunc(gl::LEQUAL);
    gl::DepthRange(1000.0f, -1000.0f);

    // gl::ColorMask(gl::FALSE_, gl::FALSE_, gl::FALSE_, gl::FALSE_);
    gl::Enable(gl::CULL_FACE);

    //* collect shit
    auto obstacles = scene.environment->getObstacles();

    //* view matrix
    //* retrieve world size from somevhere, 100x100 map looks ok for now
    auto mat = glm::ortho(-150, 150, -150, 150, 1000, -1000);

    scene.environment->vao.bind();

    //* render shit
    auto shader = assets::bindShader("PathfinderDepthGeneration");
    shader.uniform("uProjectionView", mat);
    auto obstacleTransform = shader.location("uModel");
    for(auto obstacle : obstacles){
        auto &mesh = obstacle->graphic.mesh;
        shader.uniform(obstacleTransform, obstacle->physics.transform);
        gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, mesh.offset());
    }

    gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, 0);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);

    gl::Finish();

    //* save texture
    inout.heightmap.resize(width*height);
    // gl::BindTexture(gl::TEXTURE_2D, texture.ID);
    // gl::GetTexImage(gl::TEXTURE_2D, 0, gl::RED, gl::FLOAT, (void*)inout.heightmap.data());
    // gl::BindTexture(gl::TEXTURE_2D, 0);
    gl::BindFramebuffer(gl::READ_FRAMEBUFFER, fbo);
    gl::FramebufferTexture2D(gl::READ_FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::TEXTURE_2D, texture.ID, 0);
    gl::ReadPixels(0, 0, width, height, gl::DEPTH_COMPONENT, gl::FLOAT, (void*)inout.heightmap.data());
    gl::BindFramebuffer(gl::READ_FRAMEBUFFER, 0);

    //* cleanup
    gl::BindVertexArray(0);
    gl::DeleteFramebuffers(1, &fbo);
    log("", texture.ID);
    // gl::DeleteTextures(1, &texture.ID);

    updateMinMax(inout);
}
