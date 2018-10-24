#include "core.hpp"
#include "Assets.hpp"
#include "Camera.hpp"
#include "Context.hpp"
#include "Environment.hpp"
#include "LightRendering.hpp"
#include "LightSource.hpp"
#include "PerfTimers.hpp"
#include "Scene.hpp"
#include "Sun.hpp"
#include "Window.hpp"
// #include "GameSettings.hpp"
#include "RenderDataCollector.hpp"
#include "Color.hpp"

namespace {
int isCameraInside(const glm::vec4& position, LightSource& light){
    return glm::distance2(position, light.m_position) < light.m_falloff.distance*light.m_falloff.distance;
}
}

void LightRendering::lightPass(Scene &scene, Camera &camera){
    if(scene.sun) renderSun(*scene.sun, camera);
    renderPointLights(camera);
}

void LightRendering::renderSun(Sun& sun, Camera &camera){
    GPU_SCOPE_TIMER();
    gl::Enable(gl::BLEND);
    gl::BlendFunc(gl::ONE, gl::ONE);
    gl::Disable(gl::DEPTH_TEST);
    gl::Disable(gl::CULL_FACE);
    gl::DepthMask(gl::FALSE_);
    auto shader = assets::bindShader("Sun+Ambient+Shadow");

    shader.texture("uNormal", context.tex.gbuffer.normals, 0);
    shader.texture("uDepth", context.tex.gbuffer.depth, 1);
    shader.texture("uAlbedo", context.tex.gbuffer.color, 2);
    shader.cubeMap("uCubemap", assets::getCubeMap("Park").id, 3);
    // shader.cubeMap("uCubemap", assets::getCubeMap("SciFi").id, 3);

    // shader.atlas("uCSMCascades", context.tex.shadows.cascade.ID, 4);
    // shader.uniform("uSplitDistances", camera.frustum.splitDistances);

    shader.uniform("uInvPV", camera.invPV);
    shader.uniform("uEye", camera.position().xyz());
    shader.uniform("uPixelSize", window.pixelSize);
    // shader.uniform("uShadowMapSize", context.tex.shadows.size);
    // shader.uniform("uCSMProjection", context.tex.shadows.matrices);

    clog(camera.invPV[0], camera.invPV[1], camera.invPV[2], camera.invPV[3]);
    clog(camera.position().xyz());
    clog(window.pixelSize);

    shader.uniform("light.direction", sun.direction.xyz());
    shader.uniform("light.color", sun.color.xyz());
    shader.uniform("light.energy", 100.f);
    shader.uniform("light.lightScale", 1.f);

    context.drawScreen();

    gl::DisableVertexAttribArray(0);
    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);

    context.errors();
}
void LightRendering::renderPointLights(Camera &camera){
    GPU_SCOPE_TIMER();
    float lightScale = 1;
    // fillStencil(camera, scene, Textures::LightIntensity);
    auto &pointLights = RenderDataCollector::lights[0];
    if(not pointLights.empty()){

        // setupFBO_11(Textures::LightIntensity);
        gl::Enable(gl::BLEND);
        gl::BlendFunc(gl::ONE, gl::ONE);
        gl::Enable(gl::CULL_FACE);
        gl::DepthMask(gl::FALSE_);
        gl::Disable(gl::DEPTH_TEST);
        gl::DepthFunc(gl::GEQUAL);

        assets::getVao("Common").bind();

        auto shader = assets::getShader("LightSource_POINT").bind();
        context.cullBackFaces();
        // cullFrontFaces();
        shader.uniform("uPV", camera.PV);
        shader.uniform("uInvPV", camera.invPV);
        shader.uniform("uEye", camera.position().xyz());
        shader.uniform("uPixelSize", window.pixelSize);

        context.errors();
        shader.texture("uNormal", context.tex.gbuffer.normals, 0);
        shader.texture("uDepth", context.tex.gbuffer.depth, 1);
        auto mesh = assets::getMesh("LightSphere");
        for(auto &light : pointLights){
            int cameraInside = isCameraInside(camera.position(), *light);
            shader.uniform("uSize", light->m_falloff.distance);
            shader.uniform("uPosition", light->m_position);

            shader.uniform("light.position", light->m_position.xyz());
            shader.uniform("light.color", light->m_color.xyz());
            shader.uniform("light.fallof", light->m_falloff.distance);
            shader.uniform("light.energy", light->m_energy);

            if(cameraInside){
                context.cullFrontFaces();
                gl::DepthFunc(gl::GEQUAL);
            }
            else if(not cameraInside){
                context.cullFrontFaces();
                gl::DepthFunc(gl::ALWAYS);
                // gl::Disable(gl::CULL_FACE);
            }
            mesh.render();
            if(cameraInside != 1){
                context.cullBackFaces();
                gl::DepthFunc(gl::LEQUAL);
            }
            else if(cameraInside != 0){
                context.cullBackFaces();
                gl::DepthFunc(gl::LEQUAL);
                // gl::Enable(gl::CULL_FACE);
            }
            context.errors();
        }
        pointLights.clear();
    }
    gl::Disable(gl::CULL_FACE);
    gl::DepthFunc(gl::LEQUAL);
}
void LightRendering::hemisphericalAmbient(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    // setupFBO_11_withDepth(Textures::LightIntensity);
    gl::Enable(gl::BLEND);
    gl::BlendFunc(gl::ONE, gl::ONE);
    // gl::Enable(gl::DEPTH_TEST);
    gl::Disable(gl::DEPTH_TEST);

    gl::DepthMask(gl::FALSE_);
    gl::DepthFunc(gl::NOTEQUAL);

    auto shader = assets::getShader("HemisphericalAmbient");
    shader.bind();
    shader.texture("uNormal", context.tex.gbuffer.normals, 0);

    shader.uniform("uColorIntensity", lightIntensity);
    shader.uniform("uSkyColor", glm::vec3(0.0, 0.17, 0.24));
    shader.uniform("uGroundColor", glm::vec3(0.05, 0.05, 0.07));

    context.drawScreen();

    gl::DepthFunc(gl::LEQUAL);

    context.errors();
}
void LightRendering::renderShinyObjects(Scene &scene, Camera &camera){

}
float LightRendering::calculateLuminance(){
    // GPU_SCOPE_TIMER();
    // gl::Disable(gl::BLEND);
    // gl::Disable(gl::DEPTH_TEST);
    // gl::DepthMask(gl::FALSE_);

    // setupFBO_11(Textures::full_R16F);

    // auto shader = assets::getShader("CalculateLuminance");
    // shader.bind();
    // shader.uniform("uWindowSize", windows.size.xy());
    // shader.texture("uHdrTexture", Textures::HDRScene);

    // drawScreen();

    // Textures::full_R16F.genMipmaps();
    // float luminance = 20;
    // // unbindFBO();
    // gl::FramebufferTexture(gl::READ_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, Textures::full_R16F.ID, Textures::full_R16F.noOfMipLevels()-1);
    // gl::ReadPixels(0, 0, 1, 1, gl::RED, gl::FLOAT, &luminance);
    // // gl::BindTexture(gl::TEXTURE_2D, Textures::full_R16F.ID);
    // // gl::GetnTexImage(gl::TEXTURE_2D, Textures::full_R16F.noOfMipLevels()-1, gl::RED, gl::FLOAT, 1, &luminance);
    // luminance *= 10;
    // // CHECK_FOR_ERRORS
    // clog("luminance", luminance);

    // return luminance;
    return 1;
}
void LightRendering::compose(Camera &camera){
    GPU_SCOPE_TIMER();
    context.fbo[1].tex(context.tex.full.a).tex(context.tex.gbuffer.depth)();
    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);
    gl::DepthFunc(gl::NOTEQUAL);
    gl::ClearColor(0.0,0.0,0.0,0.0);
    gl::Clear(gl::COLOR_BUFFER_BIT);

    auto shader = assets::getShader("ComposeLightAndGBuffer");
    shader.bind();
    shader.uniform("uInvPV", camera.invPV);
    shader.uniform("uEye", camera.position().xyz());
    shader.uniform("uViewDir", camera.at.xyz());
    shader.uniform("uPixelSize", window.pixelSize);
    shader.uniform("uLightScale", 1.f);
    shader.uniform("uGamma", 2.2f); // TODO: extract from app

    // shader.texture("uNormal", Textures::normalBuffer, 0);
    // shader.texture("uDepth", Textures::depthBuffer, 1);
    shader.texture("uColor", context.tex.gbuffer.color, 0);
    shader.texture("uLight", context.tex.light.color, 1);
    shader.texture("uSpecular", context.tex.light.specular, 2);
    // shader.texture("uAO", context.tex.half.rg16b, 3);
    shader.texture("uAO", context.tex.full.rg16a, 3);

    context.drawScreen();

    gl::DepthFunc(gl::LEQUAL);

    context.tex.full.a.swap(context.tex.gbuffer.color); // old gbuffer is no longer needed

    context.errors();
}
