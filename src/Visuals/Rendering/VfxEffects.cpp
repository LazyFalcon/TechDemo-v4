#include "core.hpp"
#include "VfxEffects.hpp"
#include "Assets.hpp"
#include "Color.hpp"
#include "Context.hpp"
#include "GPUResources.hpp"
#include "PerfTimers.hpp"
#include "PlasmaEffects.hpp"
#include "RenderStructs.hpp"
#include "Window.hpp"
#include "camera-data.hpp"
#include "visuals-prepared-scene.hpp"

// #include "Projectile.hpp"
#include "Utils.hpp"

// #include "ParticleProcessor.hpp"

u32 plasmaRedColor = Color(238, 239, 192, 255);

void VfxEffects::drawOpaque(camera::Camera& camera) {
    GPU_SCOPE_TIMER();
}
void VfxEffects::drawGlare(camera::Camera& camera) {
    // GPU_SCOPE_TIMER();
    // visuals::preparedScene.insert(SSDot{glm::vec2(0), 4.f, Color(50, 255, 20, 200)});
    // /// coś z OIT by się przydało, chociażby zmiana jasności w zależności od odległości, bo jeśli jasność sumować się może w nieskończoność to słabo trochę
    // /// no tak, z kwadratem odległości maleje, ale jak to ładnie blendować żeby odległy bardzo jasny obiekt nie wpierdalał się na wierzch? za bardzo przynajmniej?
    // /// mamy dwa parametry: jasność(z koloru) i opacity(alfa), jeżu skomplikowane to będzie
    // // context.defaultVAO.bind();
    // gl::BindVertexArray(0);
    // gl::DepthMask(0);
    // gl::Disable(gl::DEPTH_TEST);
    // gl::DepthFunc(gl::LEQUAL);
    // gl::Disable(gl::CULL_FACE);
    // gl::Enable(gl::PROGRAM_POINT_SIZE);
    // gl::Enable(gl::BLEND);
    // gl::BlendFunc(gl::ONE, gl::ONE);

    // drawVolumetricLaserBeams(camera);
}
void VfxEffects::drawTransparent(camera::Camera& camera) {
    // GPU_SCOPE_TIMER();
    // visuals::preparedScene.insert(SSDot{glm::vec2(0), 4.f, Color(50, 255, 20, 200)});
    // /// coś z OIT by się przydało, chociażby zmiana jasności w zależności od odległości, bo jeśli jasność sumować się może w nieskończoność to słabo trochę
    // /// no tak, z kwadratem odległości maleje, ale jak to ładnie blendować żeby odległy bardzo jasny obiekt nie wpierdalał się na wierzch? za bardzo przynajmniej?
    // /// mamy dwa parametry: jasność(z koloru) i opacity(alfa), jeżu skomplikowane to będzie
    // // context.defaultVAO.bind();
    // gl::BindVertexArray(0);
    // gl::DepthMask(0);
    // gl::Disable(gl::DEPTH_TEST);
    // gl::DepthFunc(gl::LEQUAL);
    // gl::Disable(gl::CULL_FACE);
    // gl::Enable(gl::PROGRAM_POINT_SIZE);
    // gl::Enable(gl::BLEND);
    // gl::BlendFunc(gl::ONE, gl::ONE);

    // drawPlasmaProjeciles(camera);
    // drawPlasmaFlashes(camera);
    // drawSSDots(camera);
    // drawLines(camera);
    // drawSparks(camera);
}
void VfxEffects::drawLines(camera::Camera& camera) {
    // auto &lines = visuals::preparedScene.get<Line>();
    // if(lines.empty()) return;

    // auto shader = assets::getShader("Line").bind();

    // shader.uniform("uPV", camera.PV);
    // shader.uniform("uColor", toVec4(0xffff00ff));

    // context.getBuffer().update(lines)
    //        .attrib(0).pointer_float(4).divisor(0);

    // gl::LineWidth(2);
    // gl::Enable(gl::LINE_SMOOTH);

    // gl::DrawArrays(gl::LINES, 0, lines.size()*2);

    // lines.clear();

    // context.errors();
}

void VfxEffects::drawVolumetricLaserBeams(camera::Camera& camera) {
    // auto &beams = visuals::preparedScene.get<LaserBeam>();
    // if(beams.empty()) return;

    // auto shader = assets::getShader("BeamRadiation").bind();
    // shader.uniform("uEye", camera.position().xyz());
    // shader.uniform("uCamera", camera.at.xyz());
    // shader.uniform("uPixelSize", window.pixelSize);
    // shader.uniform("uWindowSize", window.size);
    // shader.uniform("uFovTan", (float)tan(camera.fov*0.5f));
    // shader.uniform("uPV", camera.PV);
    // shader.uniform("uInvPV", camera.invPV);
    // shader.uniform("uRefDir", camera.at);
    // shader.texture("uDepth", context.tex.gbuffer.depth, 0);
    // shader.texture("uNormals", context.tex.gbuffer.normals, 1);
    // shader.texture("uJitter", assets::getImage("SSAONoise").ID, 2);
    // for(auto& beam : beams){
    //     auto dir = glm::normalize(beam.to - beam.from);
    //     glm::vec3 perpToDir = glm::cross(camera.at.xyz(), dir.xyz());
    //     float radii = beam.size*100.f;
    //     shader.uniform("uBeamSize", beam.size);
    //     shader.uniform("uGlareSize", radii);
    //     shader.uniform("uFrom", beam.from);
    //     shader.uniform("uTo", beam.to);
    //     shader.uniform("uDir", dir);
    //     shader.uniform("uPlane", plane( glm::cross(dir.xyz(), perpToDir), beam.from));
    //     shader.uniform("uTransform", glm::translate(-dir.xyz()*radii*0.5f)*glm::inverse(glm::lookAt(beam.from.xyz(), beam.to.xyz(), glm::vec3(0,0,1)))*glm::scale(glm::vec3(radii, radii, glm::distance(beam.to, beam.from)+radii)));
    //     shader.uniform("uColor", toVec4(beam.color));

    //     // mesh.render();
    //     gl::DrawArrays(gl::POINTS, 0, 1);
    // }

    // beams.clear();
    // context.errors();
}
void VfxEffects::drawPlasmaProjeciles(camera::Camera& camera) {
    // auto &projectiles = visuals::preparedScene.get<PlasmaProjectile>();
    // if(projectiles.empty()) return;

    // auto shader = assets::getShader("PlasmaProjectile").bind();

    // for(auto &it : projectiles){
    //     shader.uniform("uColor", toVec4(plasmaRedColor));
    //     shader.uniform("uFrom", it.from);
    //     shader.uniform("uTo", it.to);
    //     shader.uniform("uBrightness", 2000.f);
    //     shader.uniform("uEye", camera.position());
    //     shader.uniform("uPV", camera.PV);

    //     gl::DrawArrays(gl::POINTS, 0, 1);
    // }

    // projectiles.clear();

    // context.errors();
}
void VfxEffects::drawPlasmaFlashes(camera::Camera& camera) {
    // auto &plasma = visuals::preparedScene.get<PlasmaFlashEvent>();
    // if(plasma.empty()) return;
    // auto shader = assets::getShader("PlasmaFlash").bind();

    // for(auto &it : plasma){
    //     shader.uniform("uColor", toVec4(plasmaRedColor));
    //     shader.uniform("uPosition", it.position);
    //     shader.uniform("uSizeA", it.sizeA);
    //     shader.uniform("uSizeB", it.sizeB);
    //     shader.uniform("uBrightness", 2000.f);
    //     shader.uniform("uEye", camera.position());
    //     shader.uniform("uUp", camera.up);
    //     shader.uniform("uRight", camera.right);
    //     shader.uniform("uAt", camera.at);
    //     shader.uniform("uPV", camera.PV);
    //     shader.uniform("uNear", camera.nearDistance);
    //     shader.uniform("uFar", camera.farDistance);
    //     shader.texture("uDepth", context.tex.gbuffer.depth);
    //     shader.uniform("uPixelSize", window.pixelSize);

    //     gl::DrawArrays(gl::POINTS, 0, 1);
    // }

    // plasma.clear();

    // context.errors();
}
void VfxEffects::drawSSDots(camera::Camera& camera) {
    // auto &dots = visuals::preparedScene.get<SSDot>();
    // if(dots.empty()) return;
    // gl::Disable(gl::DEPTH_TEST);
    // auto shader = assets::getShader("SSDot").bind();

    // for(auto &it : dots){
    //     shader.uniform("uPosition", it.position);
    //     shader.uniform("uSize", it.size);
    //     shader.uniform("uPixelSize", window.pixelSize);
    //     shader.uniform("uColor", toVec4(it.color));

    //     gl::DrawArrays(gl::POINTS, 0, 1);
    // }

    // dots.clear();

    // context.errors();
}
/// draws external container
void VfxEffects::drawSparks(camera::Camera& camera) {
    // auto &sparks = visuals::preparedScene.get<ParticleProcessor*>()[0]->get<Spark>();
    // if(sparks.empty()) return;

    // gl::Enable(gl::DEPTH_TEST);
    // gl::LineWidth(1);
    // gl::Enable(gl::LINE_SMOOTH);
    // auto shader = assets::getShader("Spark").bind();
    // // shader.texture("uColor", toVec4(it.color));
    // shader.uniform("uPV", camera.PV);
    // shader.uniform("uEye", camera.position());

    // context.getBuffer().update(sparks)
    //        .attrib(0).pointer_float(1, sizeof(Spark), (void*)offsetof(Spark, lifeLeft)).divisor(0)
    //        .attrib(1).pointer_float(1, sizeof(Spark), (void*)offsetof(Spark, temperature)).divisor(0)
    //        .attrib(2).pointer_float(4, sizeof(Spark), (void*)offsetof(Spark, from)).divisor(0)
    //        .attrib(3).pointer_float(4, sizeof(Spark), (void*)offsetof(Spark, to)).divisor(0)
    //        .attrib(4).pointer_float(4, sizeof(Spark), (void*)offsetof(Spark, velocity)).divisor(0);

    // gl::DrawArrays(gl::POINTS, 0, sparks.size());

    // context.errors();
}
