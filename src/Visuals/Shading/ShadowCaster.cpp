#include "core.hpp"
#include "ShadowCaster.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "Context.hpp"
#include "Environment.hpp"
#include "Logger.hpp"
#include "PMK.hpp"
#include "PerfTimers.hpp"
#include "Scene.hpp"
#include "SceneGraph.hpp"
#include "Sun.hpp"
#include "camera-data.hpp"
#include "camera-frustum.hpp"

void ShadowCaster::prepareForDirectionalShadows(Scene& scene, camera::Camera& camera) {
    if(context.tex.shadows.cascade.ID == 0)
        initShadowMapCascade();

    gl::BindVertexArray(0);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, 0);
    gl::BindTexture(gl::TEXTURE_2D_ARRAY, 0);

    gl::BindFramebuffer(gl::FRAMEBUFFER, context.fbo.shadowmap);
    gl::FramebufferTexture(gl::DRAW_FRAMEBUFFER, gl::DEPTH_ATTACHMENT, context.tex.shadows.cascade.ID, 0);
    gl::DrawBuffers(0, &context.fbo.drawBuffers[0]);
    gl::Viewport(0, 0, context.tex.shadows.size, context.tex.shadows.size);
    gl::ClearDepth(1);
    gl::Clear(gl::DEPTH_BUFFER_BIT);

    gl::Enable(gl::DEPTH_TEST);
    gl::DepthMask(gl::TRUE_);
    gl::Disable(gl::BLEND);

    gl::DepthFunc(gl::LEQUAL);
    gl::DepthRange(0.0f, 1.0f);

    gl::ColorMask(gl::FALSE_, gl::FALSE_, gl::FALSE_, gl::FALSE_);
    gl::Disable(gl::CULL_FACE);

    camera.frustum.splitForCSM(numberOfFrustumSplits);
    calculateShadowProjectionMatrices(camera.frustum.slices, scene.sun->direction, *scene.sun, camera);

    context.errors();
}
void ShadowCaster::finishForDirectionalShadows() {
    gl::ColorMask(gl::TRUE_, gl::TRUE_, gl::TRUE_, gl::TRUE_);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, 0);
    gl::BindTexture(gl::TEXTURE_2D_ARRAY, 0);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
}
void ShadowCaster::renderScene(Scene& scene, camera::Camera& camera) {
    GPU_SCOPE_TIMER();
    if(not scene.environment)
        return;

    auto shader = assets::getShader("EnvironmentEntityShadow");
    shader.bind();

    shader.uniform("uMatrices", context.tex.shadows.matrices);

    gl::BindVertexArray(0);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);

    shader.uniform("uPV", (camera.getPV()));
    auto uModel = shader.location("uModel");

    scene.environment->vao.bind();
    // ! no longer used
    // for(auto &obj : scene.graph->visibleObjectsByType[Type::Enviro]){
    //     auto &env = scene.environment->m_entities[0]; // !!!!!!!
    //     auto &mesh = env->graphic.mesh;
    //     shader.uniform(uModel, env->physics.transform);
    //     gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, mesh.offset());
    // }

    context.errors();
}
void ShadowCaster::renderTerrain(Scene& scene, camera::Camera& camera) {
    GPU_SCOPE_TIMER();
    auto chunksToRender = getTerrainToRender(*scene.graph);

    auto shader = assets::getShader("TerrainShadowCast");
    shader.bind();

    shader.uniform("uMatrices", context.tex.shadows.matrices);

    assets::getVao("Terrain").bind();

    for(auto& it : chunksToRender) { it.render(); }

    gl::BindVertexArray(0);
    context.errors();
}

void ShadowCaster::initShadowMapCascade() {
    context.tex.shadows.cascade =
        Texture(gl::TEXTURE_2D_ARRAY, gl::DEPTH_COMPONENT32F, context.tex.shadows.size, context.tex.shadows.size,
                numberOfFrustumSplits, gl::DEPTH_COMPONENT, gl::FLOAT, gl::LINEAR, 0);

    gl::GenFramebuffers(1, &context.fbo.shadowmap);
    gl::BindFramebuffer(gl::FRAMEBUFFER, context.fbo.shadowmap);
    gl::Viewport(0, 0, context.tex.shadows.size, context.tex.shadows.size);
    gl::DrawBuffers(0, context.fbo.drawBuffers);
    gl::Enable(gl::DEPTH_TEST);
    gl::FramebufferTextureLayer(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT, context.tex.shadows.cascade.ID, 0, 0);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    context.errors();
}

std::vector<Mesh> ShadowCaster::getTerrainToRender(SceneGraph& sg) {
    std::vector<Mesh> out;

    // TODO: przerobić tak by były renderowane te które mogą zasłonić
    for(auto& it : sg.cells) { out.push_back(it.terrainMesh); }

    return out;
}

void minMaxInLightSpace(glm::vec4& bbMin, glm::vec4& bbMax, glm::vec4 position, const glm::mat4& transform) {
    position = transform * position;
    bbMin = pmk::min(position, bbMin);
    bbMax = pmk::max(position, bbMax);
};

glm::mat4 frustrumSliceProjectionViewMatrix(const camera::FrustmCorners& slices, int index, Sun& sun,
                                            camera::Camera& camera, float minZ, float maxZ) {
    glm::vec4 shadowCenter(0);
    for(auto i = index * 4; i < index * 4 + 8; i++) { shadowCenter += slices.point[i]; }
    shadowCenter /= shadowCenter.w;
    auto transform = glm::lookAt(shadowCenter.xyz() - direction.xyz(), shadowCenter.xyz(), glm::vec3(0, 0, 1));

    auto bbMin = glm::vec4(100000);
    auto bbMax = glm::vec4(-100000);

    for(auto i = index * 4; i < index * 4 + 8; i++) { minMaxInLightSpace(bbMin, bbMax, slices.point[i], transform); }

    auto lightProjectionViewMatrix = glm::ortho(bbMin.x, bbMax.x, bbMin.y, bbMax.y, -bbMax.z, -bbMin.z) * sun.transform;

    return lightProjectionViewMatrix;
}

void ShadowCaster::calculateShadowProjectionMatrices(const camera::FrustmCorners& slices, glm::vec4 light, Sun& sun,
                                                     camera::Camera& camera) {
    context.tex.shadows.matrices.clear();
    std::array<glm::vec4, 20> transformedSlices;
    std::transform(slices.point.begin(), slices.point.end(), transformedSlices.begin(),
                   [&sun](const glm::vec4& in) { return sun.transform * in; });

    for(auto i = 0; i < numberOfFrustumSplits; i++) {
        context.tex.shadows.matrices.push_back(frustrumSliceProjectionViewMatrix(slices, i, sun, camera));
    }
}

void ShadowCaster::updateShadows() {}
