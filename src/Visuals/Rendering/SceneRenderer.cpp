#include "core.hpp"
#include "SceneRenderer.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "Context.hpp"
#include "Environment.hpp"
#include "GraphicComponent.hpp" // TODO: rename
#include "Grass.hpp"
#include "Logger.hpp"
#include "PerfTimers.hpp"
#include "Scene.hpp"
#include "Sun.hpp"
#include "camera-data.hpp"
#include "visuals-prepared-scene.hpp"

void SceneRenderer::renderSceneStuff(Scene& scene, camera::Camera& camera) {
    // GPU_SCOPE_TIMER();
    // ! not used anymore, scene is rendered via indirect draw call formed from renderData gloabal
    // if(not scene.environment) return;
    // gl::Enable(gl::CULL_FACE);

    // auto shader = assets::bindShader("simple-model-pbr");

    // shader.uniform("uPV", (camera.getPV()));
    // auto uModel = shader.location("uModel");
    // shader.atlas("uAlbedo", assets::getAlbedoArray("Materials").id, 0);
    // shader.atlas("uRoughnessMap", assets::getRoughnessArray("Materials").id, 1);
    // shader.atlas("uMetallicMap", assets::getMetalic("Materials").id, 2);

    // scene.environment->vao.bind();

    // console.clog("-> Visible objects:", scene.graph->visibleObjectsByType[Type::Enviro].size());

    // // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
    // // TODO: scene renderer should have known nothing about scene and other, scene objects should add itself to proper queue. this will allow to have different materials systems in scene
    // for(auto &obj : scene.graph->visibleObjectsByType[Type::Enviro]){
    //     auto &env = scene.environment->m_entities[0];
    //     auto &mesh = env->graphic.mesh;
    //     shader.uniform(uModel, env->physics.transform);
    //     gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, mesh.offset());
    //     console.clog("->", mesh.count, mesh.offset(), env->physics.position);
    // }

    // // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
    // gl::BindVertexArray(0);
    // gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    // gl::BindTexture(gl::TEXTURE_2D, 0);

    // context.errors();
}
void SceneRenderer::renderTerrain(Scene& scene, camera::Camera& camera) {
    // ! no longer used
    // GPU_SCOPE_TIMER();
    // if(not scene.graph) return;

    // gl::Enable(gl::CULL_FACE);
    // auto shader = assets::getShader("Terrain").bind();
    // shader.uniform("uPV", camera.getPV());

    // scene.graph->vao.bind();

    // // TODO: use array to render
    // // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
    // for(auto &it : scene.graph->visibleObjectsByType[Type::TerrainChunk]){
    //     scene.graph->cells[0]->terrainMesh.render(); // !!!!!!!!!!!!!!!!!!!!!
    // }
    // // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);

    // gl::BindVertexArray(0);
    // gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    // gl::BindTexture(gl::TEXTURE_2D, 0);

    // context.errors();
}
void SceneRenderer::renderFoliage(Scene& scene, camera::Camera& camera) {
    GPU_SCOPE_TIMER();
    if(not scene.foliage)
        return;
    glm::vec4 lightDir(1, 1, -1, 0);
    if(scene.sun)
        lightDir = scene.sun->direction;

    gl::Disable(gl::BLEND);
    gl::Disable(gl::CULL_FACE);

    auto& foliage = *scene.foliage;

    foliage.meshVAO.bind();
    {
        auto shader = assets::getShader("Foliage");
        shader.bind();
        shader.atlas("uTextures", foliage.textureAtlasID);
        shader.uniform("uPV", camera.getPV());
        shader.uniform("uTime", /*time*/ 0);
        shader.uniform("uLightDirection", lightDir);
        shader.uniform("uEyeVec", camera.at);
        shader.uniform("uEye", camera.position().xyz());

        auto& data = foliage.treeBatchData;
        for(u32 i = 0; i < data.count; i++) {
            shader.uniform("uTransform", data.uniforms[i].transform);
            shader.uniform("uMotion", data.uniforms[i].motion);

            gl::DrawElements(gl::TRIANGLES, data.crown.counts[i], gl::UNSIGNED_INT,
                             (void*)(sizeof(u32) * data.crown.start[i]));
        }
    }
    {
        auto shader = assets::getShader("Foliage");
        shader.bind();
        shader.atlas("uTextures", foliage.textureAtlasID);
        shader.uniform("uPV", camera.getPV());
        shader.uniform("uTime", /*time*/ 0);
        shader.uniform("uLightDirection", lightDir);
        shader.uniform("uEyeVec", camera.at);
        shader.uniform("uEye", camera.position().xyz());

        auto& data = foliage.treeBatchData;
        for(u32 i = 0; i < data.count; i++) {
            shader.uniform("uTransform", data.uniforms[i].transform);
            shader.uniform("uMotion", data.uniforms[i].motion);

            gl::DrawElements(gl::TRIANGLES, data.trunk.counts[i], gl::UNSIGNED_INT,
                             (void*)(sizeof(u32) * data.trunk.start[i]));
        }
    }
    gl::BindTexture(gl::TEXTURE_2D, 0);
    VAO::unbind();

    context.errors();
}
void SceneRenderer::renderGrass(Scene& scene, camera::Camera& camera) {
    GPU_SCOPE_TIMER();
    if(not scene.grass)
        return;

    gl::Disable(gl::BLEND);
    gl::Disable(gl::CULL_FACE);

    auto& grass = *scene.grass;
    auto& mesh = assets::getMesh("Grass");
    auto shader = assets::getShader("Grass");

    shader.bind();
    shader.uniform("uPV", camera.getPV());
    shader.uniform("uEye", camera.position().xyz());
    shader.uniform("uSunDir", scene.sun->direction.xyz());
    shader.uniform("uTime", /*time*/ 0);
    shader.uniform("uTerrainSize", scene.graph->size);
    shader.texture("uTexture", grass.texture.ID, 0);
    shader.texture("uTopdownTerrain", context.tex.terrainTopdownView, 1);
    shader.texture("uTopdownTerrainNormals", context.tex.terrainTopdownViewNormals, 2);

    grass.vao.bind();

    gl::DrawElementsInstanced(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, mesh.offset(), grass.getPatchCount());

    gl::BindTexture(gl::TEXTURE_2D, 0);
    gl::BindVertexArray(0);

    context.errors();
}

void SceneRenderer::renderScene(Scene& scene, camera::Camera& camera) {
    renderSkinned(camera);
    render_SimpleModelPbr(camera);
    // renderTracks(camera);
}
void SceneRenderer::renderShadows(Scene& scene, camera::Camera& camera) {
    renderSkinnedShadows(scene, camera);
}
void SceneRenderer::renderGlossyObjects(camera::Camera& camera) {}

void SceneRenderer::renderSkinned(camera::Camera& camera) {
    GPU_SCOPE_TIMER();

    auto shader = assets::bindShader("skinned-model-pbr");
    auto& skinnedMeshes = visuals::preparedScene.get<model::Skinned*>();
    int nr(0);
    console.clog("Number of skinned meshes:", skinnedMeshes.size());
    for(auto toRender : skinnedMeshes) {
        console.clog("mesh nr:", nr);
        nr++;
        auto& mesh = toRender->mesh;
        toRender->vao.bind();

        { // passing bones
            GLuint UBOBindingIndex = 0;

            // update buffer
            // gl::BindBuffer(gl::UNIFORM_BUFFER, context.ubo.matrices);
            // gl::BufferSubData(gl::UNIFORM_BUFFER, 0, sizeof(glm::mat4)*toRender->bones.size(), toRender->bones.data());
            // gl::BindBuffer(gl::UNIFORM_BUFFER, 0);
            context.ubo.update(toRender->bones);
            console.clog(__PRETTY_FUNCTION__, toRender->bones.size());
            for(auto& it : toRender->bones) { console.clog(">>", it[3]); }

            // use buffer
            shader.ubo("uBones", context.ubo.matrices, UBOBindingIndex, sizeof(glm::mat4) * 150);
        }

        shader.uniform("uProjection", camera.projection);
        shader.uniform("uView", camera.view);
        shader.uniform("uModel", identityMatrix);
        // shader.atlas("uAlbedo", assets::getAlbedoArray("Materials").id, 0);
        // shader.atlas("uNormalMap", assets::getNormalArray("Materials").id, 1);
        // shader.atlas("uRoughnessMap", assets::getRoughnessArray("Materials").id, 2);
        // shader.atlas("uMetallicMap", assets::getMetalic("Materials").id, 3);

        // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);

        // gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);

        gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, (void*)0);

        // gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
        // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
    }

    skinnedMeshes.clear();

    context.errors();

    gl::BindVertexArray(0);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, 0);
};

void SceneRenderer::renderSkinnedShadows(Scene& scene, camera::Camera& camera) {
    GPU_SCOPE_TIMER();
    glm::vec4 lightDir(1, 1, -1, 0);
    if(scene.sun)
        lightDir = scene.sun->direction;
    auto shader = assets::getShader("ObjectShadow").bind();

    auto& skinnedMeshes = visuals::preparedScene.get<model::Skinned*>();
    for(auto toRender : skinnedMeshes) {
        auto& mesh = toRender->mesh;
        toRender->vao.bind();

        { // passing bones
            GLuint UBOBindingIndex = 0;
            context.ubo.update(toRender->bones);
            shader.ubo("uBones", context.ubo.matrices, UBOBindingIndex, sizeof(glm::mat4) * context.ubo.size);
        }

        shader.uniform("uMatrices", context.tex.shadows.matrices);

        gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, (void*)0);
    }

    gl::BindVertexArray(0);
    context.errors();
};

void SceneRenderer::render_SimpleModelPbr(camera::Camera& camera) {
    // GPU_SCOPE_TIMER();
    gl::Enable(gl::CULL_FACE);

    auto shader = assets::bindShader("simple-model-pbr");

    shader.uniform("uPV", (camera.getPV()));
    shader.atlas("uAlbedo", assets::getAlbedoArray("Materials").id, 0);
    shader.atlas("uRoughnessMap", assets::getRoughnessArray("Materials").id, 1);
    shader.atlas("uMetallicMap", assets::getMetalic("Materials").id, 2);

    // TODO: in loop
    context.ubo.update(visuals::preparedScene.nonPlayableInsideFrustum.transforms.data(),
                       visuals::preparedScene.nonPlayableInsideFrustum.size);
    shader.ubo("uBones", context.ubo.matrices, 0, sizeof(glm::mat4) * context.ubo.size);

    visuals::preparedScene.nonPlayableInsideFrustum.vao.bind();

    // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
    console.clog("-> Visible objects:", visuals::preparedScene.nonPlayableInsideFrustum.size);

    // gl::MultiDrawElementsIndirect(gl::TRIANGLES, gl::UNSIGNED_INT, visuals::preparedScene.nonPlayableInsideFrustum.array.data(), visuals::preparedScene.nonPlayableInsideFrustum.size, sizeof(DrawElementsIndirectCommand));
    gl::MultiDrawElements(gl::TRIANGLES, visuals::preparedScene.nonPlayableInsideFrustum.count.data(), gl::UNSIGNED_INT,
                          visuals::preparedScene.nonPlayableInsideFrustum.indices.data(),
                          visuals::preparedScene.nonPlayableInsideFrustum.size);

    // todo: dont do this here
    // visuals::preparedScene.nonPlayableInsideFrustum.clear();

    // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
    gl::BindVertexArray(0);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, 0);

    context.errors();
}
