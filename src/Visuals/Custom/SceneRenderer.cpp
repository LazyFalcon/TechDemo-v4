#include "core.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "Camera.hpp"
#include "Context.hpp"
#include "Environment.hpp"
#include "PerfTimers.hpp"
#include "Scene.hpp"
#include "Sun.hpp"
#include "Grass.hpp"
#include "SceneRenderer.hpp"

void SceneRenderer::renderScene(Scene &scene, Camera &camera){
    // GPU_SCOPE_TIMER();

    if(not scene.environment) return;
    gl::Enable(gl::CULL_FACE);

    auto shader = assets::bindShader("simple-model-pbr");

    shader.uniform("uPV", (camera.getPV()));
    auto uModel = shader.location("uModel");
    shader.atlas("uAlbedo", assets::getAlbedoArray("Materials").id, 0);
    shader.atlas("uRoughnessMap", assets::getRoughnessArray("Materials").id, 1);
    shader.atlas("uMetallicMap", assets::getMetalic("Materials").id, 2);

    scene.environment->vao.bind();

    clog("-> Visible objects:", scene.graph->visibleObjectsByType[Type::Enviro].size());

    // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
    // TODO: scene renderer should have known nothing about scene and other, scene objects should add itself to proper queue. this will allow to have different materials systems in scene
    for(auto &obj : scene.graph->visibleObjectsByType[Type::Enviro]){
        auto &env = scene.environment->entities[obj.userID];
        auto &mesh = env.graphic.mesh;
        shader.uniform(uModel, env.physics.transform);
        gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, mesh.offset());
        clog("->", mesh.count, mesh.offset(), env.physics.position);
    }

    // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
    gl::BindVertexArray(0);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, 0);

    context.errors();
}
void SceneRenderer::renderTerrain(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    if(not scene.graph) return;

    gl::Enable(gl::CULL_FACE);
    auto shader = assets::getShader("Terrain").bind();
    shader.uniform("uPV", camera.getPV());

    scene.graph->vao.bind();

    // TODO: use array to render
    // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
    for(auto &it : scene.graph->visibleObjectsByType[Type::TerrainChunk]){
        scene.graph->cells[it.userID].terrainMesh.render();
    }
    // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);

    gl::BindVertexArray(0);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, 0);

    context.errors();
}
void SceneRenderer::renderFoliage(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    if(not scene.foliage) return;
    glm::vec4 lightDir(1,1,-1, 0);
    if(scene.sun) lightDir = scene.sun->getVector();

    gl::Disable(gl::BLEND);
    gl::Disable(gl::CULL_FACE);

    auto &foliage = *scene.foliage;

    foliage.meshVAO.bind();
    {
        auto shader = assets::getShader("Foliage");
        shader.bind();
        shader.atlas("uTextures", foliage.textureAtlasID);
        shader.uniform("uPV", camera.getPV());
        shader.uniform("uTime",  /*time*/0);
        shader.uniform("uLightDirection", lightDir);
        shader.uniform("uEyeVec", camera.at);
        shader.uniform("uEye", camera.position().xyz());

        auto &data = foliage.treeBatchData;
        for(u32 i=0; i<data.count; i++){
            shader.uniform("uTransform", data.uniforms[i].transform);
            shader.uniform("uMotion", data.uniforms[i].motion);

            gl::DrawElements(gl::TRIANGLES, data.crown.counts[i], gl::UNSIGNED_INT, (void*)(sizeof(u32)*data.crown.start[i]));
        }
    }
    {
        auto shader = assets::getShader("Foliage");
        shader.bind();
        shader.atlas("uTextures", foliage.textureAtlasID);
        shader.uniform("uPV", camera.getPV());
        shader.uniform("uTime", /*time*/0);
        shader.uniform("uLightDirection", lightDir);
        shader.uniform("uEyeVec", camera.at);
        shader.uniform("uEye", camera.position().xyz());

        auto &data = foliage.treeBatchData;
        for(u32 i=0; i<data.count; i++){
            shader.uniform("uTransform", data.uniforms[i].transform);
            shader.uniform("uMotion", data.uniforms[i].motion);

            gl::DrawElements(gl::TRIANGLES, data.trunk.counts[i], gl::UNSIGNED_INT, (void*)(sizeof(u32)*data.trunk.start[i]));
        }
    }
    gl::BindTexture(gl::TEXTURE_2D, 0);
    VAO::unbind();

    context.errors();
}
void SceneRenderer::renderGrass(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    if(not scene.grass) return;

    gl::Disable(gl::BLEND);
    gl::Disable(gl::CULL_FACE);

    auto &grass = *scene.grass;
    auto &mesh = assets::getMesh("Grass");
    auto shader = assets::getShader("Grass");

    shader.bind();
    shader.uniform("uPV", camera.getPV());
    shader.uniform("uEye", camera.position().xyz());
    shader.uniform("uSunDir", scene.sun->getLightVector().xyz());
    shader.uniform("uTime", /*time*/0);
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
