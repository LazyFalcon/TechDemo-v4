#include "Context.hpp"
#include "common.hpp"
#include "Effects.hpp"
#include "PerfTimers.hpp"
#include "Assets.hpp"
#include "Camera.hpp"
#include "RendererUtils.hpp"
#include "Window.hpp"
#include "BaseStructs.hpp"
#include "Scene.hpp"
#include "Atmosphere.hpp"
#include "Starfield.hpp"
#include "Sun.hpp"
#include "DecalsAndMarkers.hpp"

std::vector<Decal> Decal::decalList;

void Effects::scattering(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    if(not scene.atmosphere || not scene.sun) return;

    gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, context.tex.full.a.ID, 0);

    auto &atmosphere = *scene.atmosphere;
    auto &sun = *scene.sun;

    gl::Disable(gl::CULL_FACE);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);
    gl::DepthFunc(gl::NOTEQUAL);
    gl::Disable(gl::BLEND);
    gl::BlendFunc(gl::ONE, gl::ZERO);
    auto shader = assets::getShader("Scattering");
    shader.bind();

    shader.uniform("uLightDirection", sun.getLightVector());
    shader.uniform("uWaveLength", 1.f/glm::pow4(atmosphere.invWavelength));
    shader.uniform("uRayleigh", atmosphere.rayleigh*4*pi);
    shader.uniform("uMie", atmosphere.mie*4*pi);
    shader.uniform("uRayleighSun", atmosphere.rayleigh*sun.power);
    shader.uniform("uMieSun", atmosphere.mie*sun.power);
    shader.uniform("uSkyRadius", atmosphere.skyRadius);
    shader.uniform("uGroundRadius", atmosphere.groundRadius);
    shader.uniform("uScaleDepth", atmosphere.scaleDepth);
    shader.uniform("uCameraHeight", atmosphere.cameraHeight);

    shader.texture("uDepth", context.tex.gbuffer.depth, 0);
    shader.texture("uColor", context.tex.gbuffer.color, 1);

    shader.uniform("uNear", camera.nearDistance);
    shader.uniform("uFar", camera.farDistance);
    shader.uniform("uEyePosition", camera.position);
    shader.uniform("uInvPV", camera.invPV);

    gl::Uniform4fv(gl::GetUniformLocation(shader.ID, "uCameraVectors"), 5, (GLfloat *)camera.frustum.cornerVectors.data());

    context.drawScreen();

    gl::DepthFunc(gl::LEQUAL);

    context.tex.full.a.swap(context.tex.gbuffer.color);

    context.errors();
}
void Effects::scatteringShadowed(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    if(not scene.atmosphere || not scene.sun) return;

    gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, context.tex.full.a.ID, 0);

    auto &atmosphere = *scene.atmosphere;
    auto &sun = *scene.sun;

    gl::Disable(gl::CULL_FACE);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);
    gl::DepthFunc(gl::NOTEQUAL);
    gl::Disable(gl::BLEND);
    gl::BlendFunc(gl::ONE, gl::ZERO);
    auto shader = assets::getShader("ScatteringShadowed");
    shader.bind();

    shader.uniform("uLightDirection", sun.getLightVector());
    shader.uniform("uWaveLength", 1.f/glm::pow4(atmosphere.invWavelength));
    shader.uniform("uRayleigh", atmosphere.rayleigh*4*pi);
    shader.uniform("uMie", atmosphere.mie*4*pi);
    shader.uniform("uRayleighSun", atmosphere.rayleigh*sun.power);
    shader.uniform("uMieSun", atmosphere.mie*sun.power);
    shader.uniform("uSkyRadius", atmosphere.skyRadius);
    shader.uniform("uGroundRadius", atmosphere.groundRadius);
    shader.uniform("uScaleDepth", atmosphere.scaleDepth);
    shader.uniform("uCameraHeight", atmosphere.cameraHeight);

    shader.texture("uDepth", context.tex.gbuffer.depth, 0);
    shader.texture("uColor", context.tex.gbuffer.color, 1);

    shader.uniform("uNear", camera.nearDistance);
    shader.uniform("uFar", camera.farDistance);
    shader.uniform("uEyePosition", camera.position);
    shader.uniform("uInvPV", camera.invPV);

    shader.atlas("uCSMCascades", context.tex.shadows.cascade.ID, 2);
    shader.uniform("uSplitDistances", camera.frustum.splitDistances);
    shader.uniform("uPixelSize", window.pixelSize);
    shader.uniform("uShadowMapSize", context.tex.shadows.size);
    shader.uniform("uCSMProjection", context.tex.shadows.matrices);

    gl::Uniform4fv(gl::GetUniformLocation(shader.ID, "uCameraVectors"), 5, (GLfloat *)camera.frustum.cornerVectors.data());

    context.drawScreen();

    gl::DepthFunc(gl::LEQUAL);

    context.tex.full.a.swap(context.tex.gbuffer.color);

    context.errors();
}
void Effects::sky(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();

    if(not scene.atmosphere || not scene.sun) return;

    auto &atmosphere = *scene.atmosphere;
    auto &sun = *scene.sun;

    gl::DepthFunc(gl::LEQUAL);
    gl::Disable(gl::CULL_FACE);
    auto shader = assets::getShader("Sky");
    shader.bind();

    auto skyMatrix = glm::translate(identityMatrix, camera.position.xyz());

    shader.uniform("uPV", camera.getPV());
    shader.uniform("uModel", skyMatrix);
    shader.uniform("uPlanetMatrix", atmosphere.planetMatrix);

    auto adjustCameraHeightToWorldScale = [](float h){return h;};

    shader.uniform("uExposure", camera.exposure);
    shader.uniform("uEye", camera.position);

    shader.uniform("uLightDirection", -sun.getVector().xyz());
    shader.uniform("uInvWaveLength", 1.f/glm::pow4(atmosphere.invWavelength.xyz()));
    shader.uniform("uKr4Pi", atmosphere.rayleigh*4*pi);
    shader.uniform("uKm4Pi", atmosphere.mie*4*pi);
    shader.uniform("uKrESun", atmosphere.rayleigh*sun.power);
    shader.uniform("uKmESun", atmosphere.mie*sun.power);
    shader.uniform("uSkyRadius", atmosphere.skyRadius);
    shader.uniform("uGroundRadius", atmosphere.groundRadius);
    shader.uniform("uScale", 1/(atmosphere.skyRadius - atmosphere.groundRadius));
    shader.uniform("uScaleDepth", atmosphere.scaleDepth);
    shader.uniform("uCameraHeight", adjustCameraHeightToWorldScale(camera.position.z));


    Mesh &mesh = assets::getMesh("FullSkySphere");
    assets::getVao("Common").bind();

    // if(Global::main.graphicOptions & graphic::WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
    gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, mesh.offset());
    // if(Global::main.graphicOptions & graphic::WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);

    gl::BindVertexArray(0);

    context.errors();
}
void Effects::starfield(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    if(not scene.starfield and not scene.sun) return;
    gl::DepthMask(0);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthFunc(gl::LEQUAL);
    gl::Disable(gl::CULL_FACE);
    gl::Enable(gl::PROGRAM_POINT_SIZE);
    gl::Enable(gl::BLEND);
    gl::BlendFunc(gl::ONE, gl::ONE);
    auto shader = assets::getShader("Starfield").bind();
    auto view = camera.view;
    view[3] = glm::vec4(0,0,0,1);

    shader.uniform("uPV", camera.PV);
    shader.uniform("uSunColor", scene.sun->getColor());

    scene.starfield->starsOnGpu.bind();
    gl::DrawArrays(gl::POINTS, 0, scene.starfield->starCount);

    scene.starfield->starsOnGpu.unbind();
    gl::Disable(gl::BLEND);
    context.errors();
}
void Effects::drawDecals(Camera &camera){
    // TODO: enable culling
    auto &decals = Decal::decalList;
    if(decals.empty()) return;
    GPU_SCOPE_TIMER();

    gl::Enable(gl::CULL_FACE);
    gl::Enablei(gl::BLEND, 0);
    gl::Disablei(gl::BLEND, 1);
    gl::BlendFunci(0, gl::ONE, gl::ONE_MINUS_SRC_ALPHA);
    gl::DepthMask(gl::FALSE_);
    gl::Enable(gl::DEPTH_TEST);
    context.cullFrontFaces();

    auto shader = assets::getShader("Decal");
    shader.bind();
    shader.uniform("uPV", camera.getPV());
    shader.uniform("uInvPV", camera.invPV);
    shader.uniform("uPixelSize", window.pixelSize);
    shader.texture("uDepth", context.tex.gbuffer.depth, 0);
    shader.texture("uNormals", context.tex.gbuffer.normals, 1);
    shader.atlas("uDecals", assets::getAlbedoArray("Decals").id, 2);
    Mesh &mesh = assets::getMesh("Box");
    assets::getVao("Common").bind();
    for(auto &decal : decals){
        shader.uniform("uCubeTransform", decal.transform);
        // shader.uniform("uCubeProjection", glm::inverse(decal.transform));
        shader.uniform("uCubeProjection", glm::affineInverse(decal.transform));
        shader.uniform("uLayer", decal.layer);
        gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, mesh.offset());
    }
    decals.clear();
    gl::BindVertexArray(0);
    context.errors();

    context.cullBackFaces();
    gl::Disablei(gl::BLEND, 0);
    gl::Disable(gl::CULL_FACE);
    gl::Disable(gl::BLEND);
    gl::DepthMask(gl::TRUE_);
}
void Effects::SSAO(Camera &camera){
    GPU_SCOPE_TIMER();
    /// maybe disable if for sky? Enable depth test for not eqial 1?
    context.fbo.tex(context.tex.full.rg16a)();
    gl::Disable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);
    gl::Disable(gl::BLEND);
    gl::Disable(gl::CULL_FACE);

    if(true){
        auto shader = assets::getShader("SSAO_WS");
        shader.bind();

        shader.texture("uDepth", context.tex.gbuffer.depth, 0);
        shader.texture("uNormal", context.tex.gbuffer.normals, 1);
        shader.texture("uSSAONoise", assets::getImage("SSAONoise").ID, 2);

        shader.uniform("uWindowSize", window.size);
        shader.uniform("uPixelSize", window.pixelSize);
        shader.uniform("uFovTan", (float)tan(camera.fov*0.5f));
        shader.uniform("uNear", camera.nearDistance);
        shader.uniform("uFar", camera.farDistance);
        shader.uniform("uView", camera.view);
        shader.uniform("uInvPV", camera.invPV);
        shader.uniform("uEyePosition", camera.position);
        context.drawScreen();
    }
    if(false){
        auto shader = assets::getShader("HBAO");
        shader.bind();

        shader.texture("uDepth", context.tex.gbuffer.depth, 0);
        shader.texture("uNormal", context.tex.gbuffer.normals, 1);
        shader.texture("uSSAONoise", assets::getImage("SSAONoise").ID, 2);

        shader.uniform("uWindowSize", window.size);
        shader.uniform("uPixelSize", window.pixelSize);
        shader.uniform("uFovTan", (float)tan(camera.fov*0.5f));
        shader.uniform("uNear", camera.nearDistance);
        shader.uniform("uFar", camera.farDistance);
        shader.uniform("uView", camera.view);
        shader.uniform("uInvPV", camera.invPV);
        shader.uniform("uEyePosition", camera.position);
        context.drawScreen();
    }
    context.tex.full.rg16a.genMipmaps();
    auto blured = utils.bilateralAOBlur(context.tex.full.rg16a);

    context.errors();
}
void Effects::toneMapping(float exposure){
    GPU_SCOPE_TIMER();
    context.fbo.tex(context.tex.full.a)();
    gl::ClearColor(0.0,0.0,0.0,0.0);
    gl::Clear(gl::COLOR_BUFFER_BIT);
    gl::Disable(gl::BLEND);
    // gl::BlendFunc(gl::ZERO, gl::SRC_COLOR);
    // gl::BlendFunc(gl::ONE, gl::ONE_MINUS_SRC_ALPHA);

    auto shader = assets::getShader("ToneMapping");
    shader.bind();
    shader.texture("uCombined", context.tex.gbuffer.color, 0);
    shader.uniform("uExposure", exposure);
    shader.uniform("uGamma", 2.2f); // TODO: extract from App

    context.drawScreen();
    context.tex.full.a.swap(context.tex.gbuffer.color);
    // context.tex.gbuffer.color.genMipmaps();

    context.errors();
}
void Effects::FXAA(){
    GPU_SCOPE_TIMER();
    // if(not (window.graphicOptions & FXAA_)) return;
    gl::Disable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);
    gl::Disable(gl::BLEND);

    gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, context.tex.full.a.ID, 0);

    auto shader = assets::getShader("FXAA");
    shader.bind();

    shader.uniform("RCPFrame", window.pixelSize);
    shader.texture("uSourceTex", context.tex.gbuffer.color, 0);

    context.drawScreen();

    context.tex.full.a.swap(context.tex.gbuffer.color);

    context.errors();
}
void Effects::chromaticDistortion(glm::vec3 strenght){
    GPU_SCOPE_TIMER();
    gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, context.tex.full.a.ID, 0);

    gl::Disable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);
    gl::Disable(gl::BLEND);
    auto shader = assets::getShader("ChromaticDistortion");
    shader.bind();

    shader.uniform("uStrenght", strenght);
    shader.uniform("uPixelSize", window.pixelSize);
    shader.texture("uTexture", context.tex.gbuffer.color.ID);

    context.drawScreen();

    context.tex.full.a.swap(context.tex.gbuffer.color);

    context.errors();
}
void Effects::vignette(float r1, float r2){
    GPU_SCOPE_TIMER();
    gl::Enable(gl::BLEND);
    gl::Disable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);

    auto shader = assets::getShader("Vignette");
    shader.bind();
    shader.uniform("uRadiusMax", r1);
    shader.uniform("uRadiusMin", r2);

    context.drawScreen();

    context.errors();
}

void Effects::bloom(){
    GPU_SCOPE_TIMER();
    gl::Disable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);
    gl::Disable(gl::BLEND);

    auto brightParts = utils.extractBrightParts(context.tex.gbuffer.color);
    gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, 0, 0);
    brightParts.genMipmaps();

    auto blur12 = utils.blur12(brightParts, BlurOptions::Symmetrical);
    auto blur14 = utils.blur14(blur12, BlurOptions::Symmetrical);
    // auto blur18 = utils.blur18(blur14, BlurOptions::Symmetrical);

    context.fbo.tex(context.tex.gbuffer.color)();
    gl::Enable(gl::BLEND);
    gl::BlendFunc(gl::ONE, gl::ONE);

    auto shader = assets::getShader("ApplyFBO").bind();
    shader.texture("uTexture", blur14, 0);

    context.drawScreen();

    context.errors();
}
void Effects::bloomSpecular(){
    GPU_SCOPE_TIMER();
    gl::Disable(gl::DEPTH_TEST);
    gl::DepthMask(gl::FALSE_);
    gl::Disable(gl::BLEND);

    auto brightParts = utils.extractBrightParts(context.tex.light.specular);
    gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, 0, 0);
    brightParts.genMipmaps();

    auto blur12 = utils.blur12(brightParts, BlurOptions::Symmetrical);
    // auto blur14 = utils.blur14(blur12, BlurOptions::Symmetrical);
    // auto blur18 = utils.blur18(blur14, BlurOptions::Symmetrical);

    context.fbo.tex(context.tex.gbuffer.color)();
    gl::Enable(gl::BLEND);
    gl::BlendFunc(gl::ONE, gl::ONE);

    auto shader = assets::getShader("ApplyFBO").bind();
    shader.texture("uTexture", blur12, 0);

    context.drawScreen();

    context.errors();
}
void Effects::matcap(Camera &camera){
    gl::Disable(gl::BLEND);
    gl::Disable(gl::DEPTH_TEST);
    gl::Disable(gl::CULL_FACE);
    gl::DepthMask(gl::FALSE_);

    auto shader = assets::getShader("Matcap");
    shader.bind();

    gl::Uniform4fv(gl::GetUniformLocation(shader.ID, "uCameraVectors"), 5, (GLfloat *)camera.frustum.cornerVectors.data());
    shader.uniform("uView", camera.view);
    shader.uniform("uInvPV", camera.invPV);

    // shader.texture("uMatcapTexture", assets::getImage(Global::m_settings["Matcap"].string()).ID, 0); // TODO: add matcap to settings?
    shader.texture("uNormals", context.tex.gbuffer.normals.ID, 1);

    context.getRandomBuffer().update(camera.frustum.cornerVectors.data(), 5*4).attrib(1).pointer_float(4).divisor(0);
    context.drawScreen();

    gl::DisableVertexAttribArray(0);
    gl::Disable(gl::BLEND);
    gl::Enable(gl::DEPTH_TEST);
    gl::DepthMask(gl::TRUE_);
    context.errors();
}
