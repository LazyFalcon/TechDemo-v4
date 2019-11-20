#include "core.hpp"
#include "visuals-csm.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "Context.hpp"
#include "Environment.hpp"
#include "Logger.hpp"
#include "PMK.hpp"
#include "PerfTimers.hpp"
#include "Scene.hpp"
#include "SceneGraph.hpp"
#include "camera-data.hpp"
#include "camera-frustum.hpp"
#include "visuals-prepared-scene.hpp"

namespace visuals
{
void CascadedShadowMapping::init() {
    context.tex.shadows.cascade =
        Texture(gl::TEXTURE_2D_ARRAY, gl::DEPTH_COMPONENT32F, context.tex.shadows.size, context.tex.shadows.size,
                m_numberOfSlices, gl::DEPTH_COMPONENT, gl::FLOAT, gl::LINEAR, 0);

    gl::GenFramebuffers(1, &context.fbo.shadowmap);
    gl::BindFramebuffer(gl::FRAMEBUFFER, context.fbo.shadowmap);
    gl::Viewport(0, 0, context.tex.shadows.size, context.tex.shadows.size);
    gl::DrawBuffers(0, context.fbo.drawBuffers);
    gl::Enable(gl::DEPTH_TEST);
    gl::FramebufferTextureLayer(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT, context.tex.shadows.cascade.ID, 0, 0);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    context.errors();
}

void CascadedShadowMapping::prepare(const MainLightParams& light, camera::Camera& camera) {
    if(context.tex.shadows.cascade.ID == 0)
        init();

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

    camera.frustum.splitForCSM(m_numberOfSlices);
    calculateShadowProjectionMatrices(camera.frustum.slices, light);

    context.errors();
}

void minMaxInLightSpace(glm::vec4& bbMin, glm::vec4& bbMax, glm::vec4 position, const glm::mat4& transform) {
    position = transform * position;
    bbMin = pmk::min(position, bbMin);
    bbMax = pmk::max(position, bbMax);
};

// todo: make position and bound values more stable in time, maybe discrete?
glm::mat4 frustrumSliceProjectionViewMatrix(const camera::FrustmCorners& slices, int index,
                                            const MainLightParams& light) {
    glm::vec4 sliceCenter(0);
    for(auto i = index; i < index + 8; i++) { sliceCenter += slices.point[i]; }
    sliceCenter /= sliceCenter.w;

    auto transform = glm::lookAt(sliceCenter.xyz() - light.direction.xyz(), sliceCenter.xyz(), glm::vec3(0, 0, 1));

    auto bbMin = glm::vec4(100000);
    auto bbMax = glm::vec4(-100000);

    for(auto i = index; i < index + 8; i++) { minMaxInLightSpace(bbMin, bbMax, slices.point[i], transform); }

    auto lightProjectionViewMatrix = glm::ortho(bbMin.x, bbMax.x, bbMin.y, bbMax.y, -bbMax.z, -bbMin.z) * transform;

    return lightProjectionViewMatrix;
}

void CascadedShadowMapping::calculateShadowProjectionMatrices(const camera::FrustmCorners& slices,
                                                              const MainLightParams& light) {
    context.tex.shadows.matrices.clear();

    for(auto i = 0; i < m_numberOfSlices; i++) {
        context.tex.shadows.matrices.push_back(frustrumSliceProjectionViewMatrix(slices, i * m_numberOfSlices, light));
    }
}

void CascadedShadowMapping::cleanup() {
    gl::ColorMask(gl::TRUE_, gl::TRUE_, gl::TRUE_, gl::TRUE_);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, 0);
    gl::BindTexture(gl::TEXTURE_2D_ARRAY, 0);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
}

void CascadedShadowMapping::render(Scene& scene, camera::Camera& camera) {
    if(not scene.environment)
        return;

    GPU_SCOPE_TIMER();

    // if(settings.heightmapPresent) renderTerrainFromHeightmap(); // otherwise terrain is rendered as regular dummy object
    renderNonPlayableObjects();
    renderBigFoliage();
    renderObjectsFromFrustum();
    renderObjectsOutsideFrustum();

    context.errors();
}
void CascadedShadowMapping::renderTerrainFromHeightmap() {}
void CascadedShadowMapping::renderNonPlayableObjects() {
    // gl::Enable(gl::CULL_FACE);

    // auto shader = assets::bindShader("dummy-object-for-csm");

    // context.ubo.update(visuals::preparedScene.nonPlayableInsideFrustum.transforms.data(),
    //                    visuals::preparedScene.nonPlayableInsideFrustum.size);
    // shader.ubo("uBones", context.ubo.matrices, 0, sizeof(glm::mat4) * context.ubo.size);

    // visuals::preparedScene.nonPlayableInsideFrustum.vao.bind();

    // gl::MultiDrawElements(gl::TRIANGLES, visuals::preparedScene.nonPlayableOutsideFrustum.count.data(),
    //                       gl::UNSIGNED_INT, visuals::preparedScene.nonPlayableOutsideFrustum.indices.data(),
    //                       visuals::preparedScene.nonPlayableOutsideFrustum.size);

    // gl::MultiDrawElements(gl::TRIANGLES, visuals::preparedScene.nonPlayableInsideFrustum.count.data(), gl::UNSIGNED_INT,
    //                       visuals::preparedScene.nonPlayableInsideFrustum.indices.data(),
    //                       visuals::preparedScene.nonPlayableInsideFrustum.size);

    // // todo: move somewere else
    // // visuals::preparedScene.nonPlayableInsideFrustum.clear();
    // // visuals::preparedScene.nonPlayableOutsideFrustum.clear();

    // // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
    // gl::BindVertexArray(0);
    // gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    // gl::BindTexture(gl::TEXTURE_2D, 0);

    // context.errors();
}
void CascadedShadowMapping::renderBigFoliage() {}
void CascadedShadowMapping::renderObjectsFromFrustum() {}
void CascadedShadowMapping::renderObjectsOutsideFrustum() {}
}