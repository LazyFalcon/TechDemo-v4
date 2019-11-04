#include "core.hpp"
#include "CustomDraw.hpp"
#include "Constants.hpp"

namespace graphic
{
std::map<u32, std::map<u32, std::vector<CustomDrawMesh>>> CustomDraw::renderQueues;

CustomDraw::CustomDraw(const std::string& params) {
    // if(params.find("shapes") != std::string::npos){
    //     vao = Shape::shapes;
    //     models.emplace("point", Mesh{0,1,1, gl::TRIANGLE_STRIP});
    //     models.emplace("quadCenter", Mesh{1,4,4, gl::TRIANGLE_STRIP});
    //     models.emplace("quadCorner", Mesh{5,8,4, gl::TRIANGLE_STRIP});
    //     models.emplace("quadCenterUv", Mesh{9,12,4, gl::TRIANGLE_STRIP});
    //     models.emplace("quadCornerUv", Mesh{13,16,4, gl::TRIANGLE_STRIP});
    //     models.emplace("screen", Mesh{17,20,4, gl::TRIANGLE_STRIP});

    //     setProjection(Global::main.getOrthoProjection());
    // }
}

void CustomDraw::loadFromFile(const std::string& filename) {
    // std::string ymlPath;
    // std::string daePath;
    // if(not findFile("../res", filename+".yml", "-r -ext", ymlPath) or
    //    not findFile("../res", filename+".dae", "-r -ext", daePath)){
    //        console.error("D2RenderQueue no:", filename);
    //        hardPause();
    //        return;
    // }
    // auto cfg = loadYaml(ymlPath);
    // loader.open(daePath);
    // for(auto &it : cfg["ModelList"]){
    //     auto mesh = loader.beginMesh();
    //     loader.load(it.string()).toMesh();
    //     loader.endMesh(mesh);
    //     models[it.string()] = mesh;
    // }
}

void CustomDraw::finalize() {
    // vao = loader.build();
}

CustomDraw& CustomDraw::model(const std::string& name) {
    // collectedDrawMesh.mesh = models[name];
    // collectedDrawMesh.tr = defaultProjection;
    return *this;
}
CustomDraw& CustomDraw::shape(const std::string& name) {
    // collectedDrawMesh.mesh = models[name];
    // collectedDrawMesh.tr = defaultProjection;
    return *this;
}

// CustomDraw& CustomDraw::position(glm::vec2 p){
//     collectedDrawMesh.tr = collectedDrawMesh.tr * glm::translate(glm::vec3(p, -0.50f));
//     return *this;
// }
// CustomDraw& CustomDraw::position(glm::vec4 p){
//     collectedDrawMesh.tr = collectedDrawMesh.tr * glm::translate(p.xyz());
//     return *this;
// }
// CustomDraw& CustomDraw::rotation(float r){
//     collectedDrawMesh.tr = collectedDrawMesh.tr * glm::rotate(r, Z3);
//     return *this;
// }
// CustomDraw& CustomDraw::scale(float s){
//     collectedDrawMesh.tr = collectedDrawMesh.tr * glm::scale(glm::vec3(s));
//     return *this;
// }
// CustomDraw& CustomDraw::scale(glm::vec3 s){
//     collectedDrawMesh.tr = collectedDrawMesh.tr * glm::scale(s);
//     return *this;
// }
// CustomDraw& CustomDraw::size(glm::vec2 s){
//     collectedDrawMesh.tr = collectedDrawMesh.tr * glm::scale(glm::vec3(s,1));
//     return *this;
// }
// CustomDraw& CustomDraw::transform(const glm::mat4 &tr){
//     collectedDrawMesh.tr = tr;
//     return *this;
// }
// CustomDraw& CustomDraw::detptMask(){
//     defaultParams |= DepthMask;
//     return *this;
// }
// CustomDraw& CustomDraw::detptTest(){
//     defaultParams |= DepthTest;
//     return *this;
// }
// CustomDraw& CustomDraw::texture(const std::string&){

//     return *this;
// }
// CustomDraw& CustomDraw::texture(u32 id){
//     defaultParams |= Texture;
//     collectedDrawMesh.textureId = id;
//     return *this;
// }
// CustomDraw& CustomDraw::textureArray(u32 id, float layer){
//     defaultParams |= TextureArrayLayer;
//     collectedDrawMesh.textureId = id;
//     collectedDrawMesh.layer = layer;
//     return *this;
// }
// CustomDraw& CustomDraw::color(u32 c){
//     collectedDrawMesh.color = c;
//     return *this;
// }
// CustomDraw& CustomDraw::operator ()(){
//     renderQueues[vao.ID][defaultParams].push_back(collectedDrawMesh);
//     collectedParams = 0;
//     collectedDrawMesh = {};
//     return *this;
// }

// void CustomDraw::render(){
//     for(auto &perVao : renderQueues){
//         gl::BindVertexArray(perVao.first);
//         for(auto &perRenderSetup : perVao.second){
//             auto renderSetup = perRenderSetup.first;
//             Shader shader;
//             i32 colorTexId;
//             i32 uModel;
//             if(renderSetup & DepthTest)
//                 gl::Enable(gl::DEPTH_TEST);
//             else
//                 gl::Disable(gl::DEPTH_TEST);
//             if(renderSetup & DepthMask)
//                 gl::DepthMask(gl::TRUE_);
//             else
//                 gl::DepthMask(gl::FALSE_);
//             if(renderSetup & Texture){
//                 shader = assets::getShader("SimpleRenderTextured");
//                 colorTexId = shader.location("uTexture");
//             }
//             else if(renderSetup & TextureArrayLayer){
//                 shader = assets::getShader("SimpleRenderTextureLayer");
//                 colorTexId = shader.location("uTexture");
//             }
//             else {
//                 shader = assets::getShader("SimpleRenderNoUV");
//                 colorTexId = shader.location("uColor");
//             }
//             shader.bind();
//             uModel = shader.location("uModel");

//             for(auto &it : perRenderSetup.second){
//                 (renderSetup & Texture) ? shader.texture(colorTexId, it.textureId) : shader.uniform(colorTexId, colorHex(it.color));
//                 shader.uniform(uModel, it.tr);
//                 if(renderSetup & TextureArrayLayer) shader.uniform("uLayer", it.layer);
//                 it.mesh.render();
//             }
//             perRenderSetup.second.clear();

//             // resetGlStates();
//             CHECK_FOR_ERRORS
//         }
//     }
//     gl::BindVertexArray(0);
// }

}
