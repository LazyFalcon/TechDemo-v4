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
        if(it > max) max = it;
    }

    inout.min = min;
    inout.max = max;
    log("map min/max:", min, max);
}

float zFar = 150;
float zNear = -150;

void unprojectDepth(ResultMap& inout){
    for(auto & z : inout.heightmap) z = (2*z-1) * (zNear - zFar)/2.f;
}

Texture texture;
Texture textureColor;
uint fbo;

void pathifinderProcessAndSaveDepthMap(Scene& scene, Context& context, ResultMap& inout){
    int width(inout.width), height(inout.height);
    //* create texture
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    textureColor = Texture(gl::TEXTURE_2D, gl::RGBA8, width, height, 1, gl::RGBA, gl::UNSIGNED_INT, gl::LINEAR, 0);
    texture = Texture(gl::TEXTURE_2D, gl::DEPTH_COMPONENT32F, width, height, 1, gl::DEPTH_COMPONENT, gl::FLOAT, gl::LINEAR, 0);
    // auto texture = Texture(gl::TEXTURE_2D, gl::DEPTH_COMPONENT32F, width, height, 1, gl::DEPTH_COMPONENT, gl::FLOAT, gl::LINEAR, 0);

    //* create framebuffer
    gl::GenFramebuffers(1, &fbo);
    gl::BindFramebuffer(gl::FRAMEBUFFER, fbo);
    gl::Viewport(0, 0, width, height);
    gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, textureColor.ID, 0);
    gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::TEXTURE_2D, texture.ID, 0);
    gl::DrawBuffers(1, context.fbo.drawBuffers);

    gl::Enable(gl::DEPTH_TEST);
    gl::DepthMask(gl::TRUE_);
    gl::Disable(gl::BLEND);
    gl::Enable(gl::CULL_FACE);

    gl::DepthFunc(gl::LEQUAL);
    gl::DepthRange(0.0f, 1.0f);

    gl::ClearDepth(1.0);
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    // gl::ColorMask(gl::FALSE_, gl::FALSE_, gl::FALSE_, gl::FALSE_);
    context.errors();

    //* collect shit
    auto obstacles = scene.environment->getObstacles();

    //* view matrix
    //* retrieve world size from somevhere, 100x100 map looks ok for now
    auto mat = glm::ortho(-150.f, 150.f, -150.f, 150.f, zNear, zFar);
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

    //* save texture
    inout.heightmap.resize(width*height);
    gl::BindFramebuffer(gl::READ_FRAMEBUFFER, fbo);
    gl::FramebufferTexture2D(gl::READ_FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::TEXTURE_2D, texture.ID, 0);
    gl::ReadPixels(0, 0, width, height, gl::DEPTH_COMPONENT, gl::FLOAT, (void*)inout.heightmap.data());
    gl::BindFramebuffer(gl::READ_FRAMEBUFFER, 0);

    //* cleanup
    gl::BindVertexArray(0);
    gl::DeleteFramebuffers(1, &fbo);
    gl::DeleteTextures(1, &texture.ID);
    gl::DeleteTextures(1, &textureColor.ID);

    unprojectDepth(inout);
    updateMinMax(inout);
}
