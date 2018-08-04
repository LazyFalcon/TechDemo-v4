#include "core.hpp"
#include "ObjectBatchedRender.hpp"
#include "Context.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "GraphicComponent.hpp" // TODO: rename
#include "Logging.hpp"
#include "Camera.hpp"
#include "PerfTimers.hpp"
#include "Environment.hpp"
// #include "TrackSim.hpp"
// #include "TankDriveSystem.hpp"
#include "Scene.hpp"
#include "Sun.hpp"
#include "RenderQueue.hpp"

void ObjectBatchedRender::renderObjects(Scene &scene, Camera &camera){
    renderSkinned(camera);
    render_SimpleModelPbr(camera);
    // renderTracks(camera);
}
void ObjectBatchedRender::renderShadows(Scene &scene, Camera &camera){
    renderSkinnedShadows(scene, camera);
}
void ObjectBatchedRender::renderGlossyObjects(Camera &camera){
}


void ObjectBatchedRender::renderSkinned(Camera &camera){
    GPU_SCOPE_TIMER();

    auto shader = assets::bindShader("skinned-model-pbr");
    auto &skinnedMeshes = RenderQueue::get<SkinnedMesh*>();
    int nr(0);
    for(auto toRender : skinnedMeshes)
    {
        clog("mesh nr:",  nr);
        nr++;
        auto &mesh = toRender->mesh;
        toRender->vao.bind();

        { // passing bones
            GLuint UBOBindingIndex = 0;

            // update buffer
            gl::BindBuffer(gl::UNIFORM_BUFFER, context.ubo.matrices);
            gl::BufferSubData(gl::UNIFORM_BUFFER, 0, sizeof(glm::mat4)*toRender->bones.size(), toRender->bones.data());
            gl::BindBuffer(gl::UNIFORM_BUFFER, 0);

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

void ObjectBatchedRender::renderSkinnedShadows(Scene &scene, Camera &camera){
    GPU_SCOPE_TIMER();
    glm::vec4 lightDir(1,1,-1, 0);
    if(scene.sun) lightDir = scene.sun->direction;
    auto shader = assets::getShader("ObjectShadow").bind();

    auto &skinnedMeshes = RenderQueue::get<SkinnedMesh*>();
    for(auto toRender : skinnedMeshes)
    {
        auto &mesh = toRender->mesh;
        toRender->vao.bind();

        { // passing bones
            GLuint UBOBindingIndex = 0;
            // // update buffer
            // gl::BindBuffer(gl::UNIFORM_BUFFER, context.ubo.matrices);
            // gl::BufferSubData(gl::UNIFORM_BUFFER, 0, sizeof(glm::mat4)*toRender->bones.size(), toRender->bones.data());
            // gl::BindBuffer(gl::UNIFORM_BUFFER, 0);

            // use buffer
            context.ubo.update(toRender->bones);
            shader.ubo("uBones", context.ubo.matrices, UBOBindingIndex, sizeof(glm::mat4) * context.ubo.size);
        }

        shader.uniform("uMatrices",  context.tex.shadows.matrices);

        gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, (void*)0);
    }

    gl::BindVertexArray(0);
    context.errors();
};

void ObjectBatchedRender::render_SimpleModelPbr(Camera &camera){
    // GPU_SCOPE_TIMER();
    gl::Enable(gl::CULL_FACE);

    auto shader = assets::bindShader("simple-model-pbr");

    shader.uniform("uPV", (camera.getPV()));
    shader.atlas("uAlbedo", assets::getAlbedoArray("Materials").id, 0);
    shader.atlas("uRoughnessMap", assets::getRoughnessArray("Materials").id, 1);
    shader.atlas("uMetallicMap", assets::getMetalic("Materials").id, 2);

    // TODO: in loop
    context.ubo.update(RenderQueue::enviro.transforms.data(), RenderQueue::enviro.size);
    shader.ubo("uBones", context.ubo.matrices, 0, sizeof(glm::mat4) * context.ubo.size);

    RenderQueue::enviro.vao.bind();

    // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);
    clog("-> Visible objects:", RenderQueue::enviro.size);

    // gl::MultiDrawElementsIndirect(gl::TRIANGLES, gl::UNSIGNED_INT, RenderQueue::enviro.array.data(), RenderQueue::enviro.size, sizeof(DrawElementsIndirectCommand));
    gl::MultiDrawElements(gl::TRIANGLES, RenderQueue::enviro.count.data(), gl::UNSIGNED_INT, RenderQueue::enviro.indices.data(), RenderQueue::enviro.size);

    RenderQueue::enviro.clear();

    // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::FILL);
    gl::BindVertexArray(0);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
    gl::BindTexture(gl::TEXTURE_2D, 0);

    context.errors();
}
