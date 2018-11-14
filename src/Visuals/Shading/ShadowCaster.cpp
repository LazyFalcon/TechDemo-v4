#include "core.hpp"
#include "ShadowCaster.hpp"
#include "Logging.hpp"
#include "Camera.hpp"
#include "Context.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "PerfTimers.hpp"
#include "Scene.hpp"
#include "Sun.hpp"
#include "Frustum.hpp"
#include "SceneGraph.hpp"
#include "PMK.hpp"
#include "Environment.hpp"

void ShadowCaster::prepareForDirectionalShadows(Scene &scene, Camera &camera){
    if(context.tex.shadows.cascade.ID == 0) initShadowMapCascade();

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


    auto cornersOfSplittedFrustum = camera.frustum.splitForCSM(numberOfFrustumSplits);
    calculateShadowProjectionMatrices(cornersOfSplittedFrustum, scene.sun->direction, *scene.sun, camera);

    context.errors();
}
void ShadowCaster::finishForDirectionalShadows(){
    gl::ColorMask(gl::TRUE_, gl::TRUE_, gl::TRUE_, gl::TRUE_);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, 0);
    gl::BindTexture(gl::TEXTURE_2D_ARRAY, 0);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
}
void ShadowCaster::renderScene(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    if(not scene.environment) return;

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
void ShadowCaster::renderTerrain(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    auto chunksToRender = getTerrainToRender(*scene.graph);

    auto shader = assets::getShader("TerrainShadowCast");
    shader.bind();

    shader.uniform("uMatrices", context.tex.shadows.matrices);

    assets::getVao("Terrain").bind();

    for(auto &it : chunksToRender){
        it.render();
    }

    gl::BindVertexArray(0);
    context.errors();
}

void ShadowCaster::initShadowMapCascade(){
    context.tex.shadows.cascade = Texture(gl::TEXTURE_2D_ARRAY,
                                        gl::DEPTH_COMPONENT32F,
                                        context.tex.shadows.size,
                                        context.tex.shadows.size,
                                        numberOfFrustumSplits,
                                        gl::DEPTH_COMPONENT,
                                        gl::FLOAT,
                                        gl::LINEAR, 0);

    gl::GenFramebuffers(1, &context.fbo.shadowmap);
    gl::BindFramebuffer(gl::FRAMEBUFFER, context.fbo.shadowmap);
    gl::Viewport(0, 0, context.tex.shadows.size, context.tex.shadows.size);
    gl::DrawBuffers(0, context.fbo.drawBuffers);
    gl::Enable(gl::DEPTH_TEST);
    gl::FramebufferTextureLayer(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT, context.tex.shadows.cascade.ID, 0, 0);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    context.errors();
}

std::vector<Mesh> ShadowCaster::getTerrainToRender(SceneGraph &sg){
    std::vector<Mesh> out;

    // TODO: przerobić tak by były renderowane te które mogą zasłonić
    for(auto &it : sg.cells){
        out.push_back(it.terrainMesh);
    }

    return out;
}

glm::mat4 ShadowCaster::fitShadowProjectionAroundBoundingBox(FrustmCorners &corners, Sun &sun, Camera &camera, float minZ, float maxZ){
    glm::vec4 shadowCenter(0);
    for(auto i=0; i<8; i++){
        shadowCenter += corners.array[i];
    }
    shadowCenter /= shadowCenter.w;
    pmk::updateSunTransform(shadowCenter, camera.up);
    auto bbMin = glm::vec4(100000);
    auto bbMax = glm::vec4(-100000);
    for(auto i=0; i<8; i++){
        pmk::sunSpaceViewBox(bbMin, bbMax, corners.array[i]);
    }

    auto mat = glm::ortho(bbMin.x, bbMax.x, bbMin.y, bbMax.y, -bbMax.z, -bbMin.z) * sun.transform;

    return mat;
}
void ShadowCaster::calculateShadowProjectionMatrices(std::vector<FrustmCorners> &frustumSlices, glm::vec4 light, Sun &sun, Camera &camera){
    context.tex.shadows.matrices.clear();
    for(auto i=0; i<numberOfFrustumSplits; i++){
        context.tex.shadows.matrices.push_back(fitShadowProjectionAroundBoundingBox(
            frustumSlices[i],
            sun,
            camera
        ));
    }
}

void ShadowCaster::updateShadows(){

}
