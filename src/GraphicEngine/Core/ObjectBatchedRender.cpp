#include "ObjectBatchedRender.hpp"
#include "Context.hpp"
#include "Assets.hpp"
#include "BaseStructs.hpp"
#include "GraphicComponent.hpp" // TODO: rename
#include "Logging.hpp"
#include "Camera.hpp"
#include "PerfTimers.hpp"
// #include "TrackSim.hpp"
// #include "TankDriveSystem.hpp"
#include "Scene.hpp"
#include "Sun.hpp"
#include "RenderQueue.hpp"

void ObjectBatchedRender::renderObjects(Camera &camera){
    renderSkinned(camera);
    // renderTracks(camera);
}
void ObjectBatchedRender::renderShadows(Scene &scene, Camera &camera){
    renderSkinnedShadows(scene, camera);
}
void ObjectBatchedRender::renderGlossyObjects(Camera &camera){
}


void ObjectBatchedRender::renderSkinned(Camera &camera){
    GPU_SCOPE_TIMER();

    auto shader = assets::getShader("ObjectBody");
    shader.bind();
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
        shader.atlas("uAlbedo", assets::getAlbedoArray("Materials").id, 0);
        shader.atlas("uNormalMap", assets::getNormalArray("Materials").id, 1);
        shader.atlas("uRoughnessMap", assets::getRoughnessArray("Materials").id, 2);
        shader.atlas("uMetallicMap", assets::getMetalic("Materials").id, 3);

        // if(Global::main.graphicOptions & WIREFRAME) gl::PolygonMode(gl::FRONT_AND_BACK, gl::LINE);

        gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, (void*)0);

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
    if(scene.sun) lightDir = scene.sun->getVector();
    auto shader = assets::getShader("ObjectShadow").bind();

    auto &     skinnedMeshes = RenderQueue::get<SkinnedMesh*>();
    for(auto toRender : skinnedMeshes)
    {
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

        shader.uniform("uMatrices",  context.tex.shadows.matrices);

        gl::DrawElements(gl::TRIANGLES, mesh.count, gl::UNSIGNED_INT, (void*)0);
    }

    gl::BindVertexArray(0);
    context.errors();
};
