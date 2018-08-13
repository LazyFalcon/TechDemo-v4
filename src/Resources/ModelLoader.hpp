#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "GPUResources.hpp"
#include "Logging.hpp"
// TODO: zaoszczedzic na includach!
#include "PhysicalWorld.hpp"

struct InternalMeshInfo
{
    size_t vertexStart;
    size_t indexStart;

    u32 vertexCount;
    u32 indexCount;

    Mesh toMesh(){
        Mesh out {};
        out.begin = indexStart;
        out.end = indexStart + indexCount;
        out.count = indexCount;
        out.vertexStart = vertexStart;
        out.vertexEnd = vertexStart + vertexCount;

        return out;
    }
};

namespace vertexCopy{
    template<typename T>
    void positions(aiMesh& mesh, std::vector<T>& target, int startPosition){
        u32 count = mesh.mNumVertices;
        // u32 start = target.size();
        if(count == 0) error(mesh.mName.C_Str(), " position data not definied");
        for(u32 from=0, to=startPosition; from<count; from++, to++){
            target[to].position = {mesh.mVertices[from].x, mesh.mVertices[from].y, mesh.mVertices[from].z};
        }
    }
    template<typename T>
    void texcoords(aiMesh& mesh, std::vector<T>& target, int startPosition){
        u32 count = mesh.mNumVertices;
        if(not mesh.HasTextureCoords(0)){
            error(mesh.mName.C_Str(), " UV data not definied");

            for(u32 to=startPosition; to < startPosition+count; to++){
                target[to].uv = {0,0,0};
            }
            return;
        }

        for(u32 from=0, to=startPosition; from<count; from++, to++){
            target[to].uv = {mesh.mTextureCoords[0][from].x,mesh.mTextureCoords[0][from].y,mesh.mTextureCoords[0][from].z};
        }
    }
    template<typename T>
    void normals(aiMesh& mesh, std::vector<T>& target, int startPosition){
        u32 count = mesh.mNumVertices;
        if(not mesh.HasNormals()) error(mesh.mName.C_Str(), "normal data not definied");
        for(u32 from=0, to=startPosition; from<count; from++, to++){
            target[to].normal = {mesh.mNormals[from].x, mesh.mNormals[from].y, mesh.mNormals[from].z};
        }
    }
    template<typename T>
    void tangents(aiMesh& mesh, std::vector<T>& target, int startPosition){
        u32 count = mesh.mNumVertices;
        if(count == 0) error(mesh.mName.C_Str(), "tangent data not definied");
        for(u32 from=0, to=startPosition; from<count; from++, to++){
            target[to].tangent = {mesh.mTangents[from].x, mesh.mTangents[from].y, mesh.mTangents[from].z};
        }
    }
    template<typename T>
    void color(aiMesh& mesh, std::vector<T>& target, int startPosition, glm::vec3 materialColor){
        u32 count = mesh.mNumVertices;
        if(not mesh.HasVertexColors(0)){
            for(u32 to=startPosition; to < startPosition+count; to++){
                target[to].color = materialColor;
            }

            return;
        }
        for(u32 from=0, to=startPosition; from<count; from++, to++){
            target[to].color = {mesh.mColors[0][from].r, mesh.mColors[0][from].g, mesh.mColors[0][from].b};
        }
    }
}


struct VertexSimpleFlat
{
    glm::vec3 position;
    glm::vec3 uv;

    static void copyData(aiMesh& mesh, std::vector<VertexSimpleFlat>& data, int startPosition){
        vertexCopy::positions(mesh, data, startPosition);
        vertexCopy::texcoords(mesh, data, startPosition);
    }
    static void copyMaterial(aiMesh& mesh, std::vector<VertexSimpleFlat>& data, int startPosition, const aiScene& scene){

    }
    static void setupVao(VAO& vao, std::vector<VertexSimpleFlat>& data, std::vector<u32>& indices){
        auto size = sizeof(VertexSimpleFlat);
        auto& vbo = vao.setup().addBuffer(data.data(), data.size()*sizeof(VertexSimpleFlat));
        vbo.attrib(0).pointer_float(3, size, (void*)offsetof(VertexSimpleFlat, position)).divisor(0);
        vbo.attrib(1).pointer_float(3, size, (void*)offsetof(VertexSimpleFlat, uv)).divisor(0);
        vao.addBuffer(indices)();
    }
};

struct VertexSimple
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 uv;

    static void copyData(aiMesh& mesh, std::vector<VertexSimple>& data, int startPosition){
        vertexCopy::positions(mesh, data, startPosition);
        vertexCopy::normals(mesh, data, startPosition);
        vertexCopy::texcoords(mesh, data, startPosition);
    }
    static void copyMaterial(aiMesh& mesh, std::vector<VertexSimple>& data, int startPosition, const aiScene& scene){

    }
    static void setupVao(VAO& vao, std::vector<VertexSimple>& data, std::vector<u32>& indices){
        auto size = sizeof(VertexSimple);
        auto& vbo = vao.setup().addBuffer(data.data(), data.size()*sizeof(VertexSimple));
        vbo.attrib(0).pointer_float(3, size, (void*)offsetof(VertexSimple, position)).divisor(0);
        vbo.attrib(1).pointer_float(3, size, (void*)offsetof(VertexSimple, normal)).divisor(0);
        vbo.attrib(2).pointer_float(3, size, (void*)offsetof(VertexSimple, uv)).divisor(0);
        vao.addBuffer(indices)();
    }
};

struct VertexWithMaterialData
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 uv;
    glm::vec3 color;

    static void copyData(aiMesh& mesh, std::vector<VertexWithMaterialData>& data, int startPosition){
        vertexCopy::positions(mesh, data, startPosition);
        vertexCopy::normals(mesh, data, startPosition);
        vertexCopy::texcoords(mesh, data, startPosition);
    }

    static void copyMaterial(aiMesh& mesh, std::vector<VertexWithMaterialData>& data, int startPosition, const aiScene& scene){
        auto materialId = mesh.mMaterialIndex;
        if(scene.mNumMaterials <= materialId){
            error("For", mesh.mName.C_Str(), "scene has no material with id:", materialId);
            return;
        }

        auto& material = *scene.mMaterials[materialId];

        if(aiColor3D aicolor(0.f,0.f,0.f); AI_SUCCESS == material.Get(AI_MATKEY_COLOR_DIFFUSE, aicolor)){
            glm::vec3 color(aicolor.r, aicolor.g, aicolor.b);
            vertexCopy::color(mesh, data, startPosition, color);
        }
        else {
            error("For", mesh.mName.C_Str(), "has no color in material");
            return;
        }

    }

    static void setupVao(VAO& vao, std::vector<VertexWithMaterialData>& data, std::vector<u32>& indices){
        auto size = sizeof(VertexWithMaterialData);
        auto& vbo = vao.setup().addBuffer(data.data(), data.size()*sizeof(VertexWithMaterialData));
        vbo.attrib(0).pointer_float(3, size, (void*)offsetof(VertexWithMaterialData, position)).divisor(0);
        vbo.attrib(1).pointer_float(3, size, (void*)offsetof(VertexWithMaterialData, normal)).divisor(0);
        vbo.attrib(2).pointer_float(3, size, (void*)offsetof(VertexWithMaterialData, uv)).divisor(0);
        vbo.attrib(3).pointer_float(3, size, (void*)offsetof(VertexWithMaterialData, color)).divisor(0);
        vao.addBuffer(indices)();
    }

};

struct VertexWithMaterialDataAndBones
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 uv;
    uint boneId;
    glm::vec3 color;

    static void copyData(aiMesh& mesh, std::vector<VertexWithMaterialDataAndBones>& data, int startPosition){
        vertexCopy::positions(mesh, data, startPosition);
        vertexCopy::normals(mesh, data, startPosition);
        vertexCopy::texcoords(mesh, data, startPosition);
        // vertexCopy::color(mesh, data, startPosition);
    }

    static void copyMaterial(aiMesh& mesh, std::vector<VertexWithMaterialDataAndBones>& data, int startPosition, const aiScene& scene){

    }

    static void setupVao(VAO& vao, std::vector<VertexWithMaterialDataAndBones>& data, std::vector<u32>& indices){
        auto size = sizeof(VertexWithMaterialDataAndBones);
        auto& vbo = vao.setup().addBuffer(data.data(), data.size()*sizeof(VertexWithMaterialDataAndBones));
        vbo.attrib(0).pointer_float(3, size, (void*)offsetof(VertexWithMaterialDataAndBones, position)).divisor(0);
        vbo.attrib(1).pointer_float(3, size, (void*)offsetof(VertexWithMaterialDataAndBones, normal)).divisor(0);
        vbo.attrib(2).pointer_float(3, size, (void*)offsetof(VertexWithMaterialDataAndBones, uv)).divisor(0);
        vbo.attrib(3).pointer_integer(1, size, (void*)offsetof(VertexWithMaterialDataAndBones, boneId)).divisor(0);
        vao.addBuffer(indices)();
    }
};

/**
  *  next lod levels has suffix _lod<level[1:3]>
  *  loading inserts in collection, position in collecion is returned
  *  for loadAll models are loaded in order of LOD
  *
  *
*/
template<typename VertexFormat>
class ModelLoader
{
public:
    bool good { false };
    bool debug { false };

    struct {
        float vertex4comonent;
        std::optional<int> uvComponents;
        std::optional<int> tangents;
        std::optional<float> roughness;
        std::optional<float> metallic;
        std::optional<float> reflectivity; // * not sure if really needed
        std::optional<glm::vec2> clearCoat; // * color is none, but strenght of layer and roughnes
        std::optional<glm::vec3> emissive; // * color with intensity, directly to light
        std::optional<std::string> detailTexture;
    } defaults;
    ModelLoader(){
        log("Loader created");
    }

    ModelLoader& open(const std::string &filename, std::function<float(const std::string&)> getLayerFunction){
        getLayer = getLayerFunction;
        return open(filename);
    }
    ModelLoader& open(const std::string &filename){
        if(scene) close();
        scene = importer.ReadFile(filename.c_str()
            ,
            aiProcess_CalcTangentSpace
            | aiProcess_Triangulate
            | aiProcess_ImproveCacheLocality
            // | aiProcess_PreTransformVertices
            // | aiProcess_SortByPType
            | aiProcess_JoinIdenticalVertices
            );
        rawVertexData.reserve(100000);
        indices.reserve(100000);

        good = (bool)scene;
        return *this;
    }
    void close(){
        importer.FreeScene();
        indices.clear();
        rawVertexData.clear();
        scene = nullptr;
    }
    ~ModelLoader(){
        if(scene) close();
    }

    Mesh beginMesh(){
        Mesh mesh {};
        mesh.begin = indices.size();
        mesh.vertexStart = rawVertexData.size();
        return mesh;
    }
    Mesh endMesh(Mesh &m){
        m.end = indices.size();
        m.count = m.end - m.begin;
        m.vertexEnd = rawVertexData.size();
        return m;
    }
    /*
    * loads model with respect to division caused by materials
    */
    Mesh load(const std::string &name){
        return load(std::vector<std::string>{name});
    }
    InternalMeshInfo loadOnly(const std::string &name){
        return loadOnly(std::vector<std::string>{name});
    }

    /*
    * loads multiple models with respect to division caused by materials into one
    */
    Mesh load(const std::vector<std::string> &names){
        auto out = beginMesh();
        loadOnly(names);
        return endMesh(out);
    }
    InternalMeshInfo loadOnly(const std::vector<std::string> &names){

        InternalMeshInfo info {rawVertexData.size(), indices.size()};

        std::vector<aiMesh*> models;

        for(auto& name : names){
            auto f = findSubMeshesByMaterial(name);
            models.insert(models.end(), f.begin(), f.end());
        }

        if(models.empty()){
            error("no models:", names[0]);
            return {};
        }
        for(auto submodel : models){
            InternalMeshInfo subinfo {rawVertexData.size(), indices.size()};
            copyIndices(*submodel, indices, rawVertexData.size());
            rawVertexData.resize(rawVertexData.size() + submodel->mNumVertices);
            VertexFormat::copyData(*submodel, rawVertexData, subinfo.vertexStart);
            VertexFormat::copyMaterial(*submodel, rawVertexData, subinfo.vertexStart, *scene);

            info.indexCount = indices.size() - info.indexStart;
            info.vertexCount = rawVertexData.size() - info.vertexStart;
            // if(getLayer) setTextureLayer(subinfo, getLayer(getTextureName(mesh)));
        }
        return info;
    }
    std::vector<ConvexMesh> loadConvexMeshes(const std::vector<std::string> &modelsToJoin){
        std::vector<ConvexMesh> out;
        for(auto& name : modelsToJoin){
            auto meshes = findSubMeshesByMaterial(name);
            if(meshes.empty()){
                error("no mesh");
                return {};
            }
            for(auto mesh : meshes){
                std::vector<btScalar> data;
                data.resize(mesh->mNumVertices*3);
                for(u32 i=0, j=0; i<mesh->mNumVertices; i++){
                    data[j++] = mesh->mVertices[i].x;
                    data[j++] = mesh->mVertices[i].y;
                    data[j++] = mesh->mVertices[i].z;
                }
                out.emplace_back();
                out.back().data.swap(data);
            }
        }
        return out;
    }
    void setTextureLayer(InternalMeshInfo info, float layer){
        for(u32 i=0; i<info.vertexCount; i++){
            rawVertexData[info.vertexStart].uv[2] += layer;
        }
    }
    void setBoneIndex(InternalMeshInfo info, float index){
        for(u32 i=0; i<info.vertexCount; i++){
            rawVertexData[info.vertexStart].boneId += index;
        }
    }
    void moveModel(InternalMeshInfo info, glm::vec3 offset){
        for(u32 i=info.vertexStart; i<info.vertexCount; i++){
            rawVertexData[i].position += offset;
        }
    }
    void moveModel(const Mesh& info, glm::vec3 offset){
        for(u32 i=info.vertexStart; i<info.vertexEnd; i++){
            rawVertexData[i].position += offset;
        }
    }
    VAO build(){
        VAO vao {};
        VertexFormat::setupVao(vao, rawVertexData, indices);
        // vao.setup().addBuffer(vertex, 4);
        //     if(defaults.uvComponents) vao.addBuffer(texcoord, *defaults.uvComponents);
        //     vao.addBuffer(normal, 4);
        //     if(loadTangents) vao.addBuffer(tangent, 4);
        //     vao.addBuffer(indices)();
        return vao;
    }

    std::pair<std::vector<float>, std::vector<u32>> loadStatic3DMesh(const std::string &name){
        std::vector<float> outVerts;
        std::vector<u32> outIndices;
        if(not scene){
            error("No scene for ModelLoader");
            hardPause();
            return {};
        }

        auto meshes = findSubMeshesByMaterial(name);
        if(meshes.empty()){
            error("no mesh:", name);
            return {};
        }


        for(auto mesh : meshes){
            copyIndices(*mesh, outIndices, outVerts.size()/4);
            u32 count = mesh->mNumVertices;
            u32 start = outVerts.size();
            outVerts.resize(outVerts.size()+count*4);
            for(u32 i=0, j=0; i<count; i++, j+=4){
                outVerts[start+j] = mesh->mVertices[i].x;
                outVerts[start+j+1] = mesh->mVertices[i].y;
                outVerts[start+j+2] = mesh->mVertices[i].z;
                outVerts[start+j+3] = 0;
            }
        }

        return std::make_pair(outVerts, outIndices);
    }

    std::vector<std::string> getNames(){
        std::vector<std::string> out;

        for(int i=0; i<scene->mNumMeshes; i++){
            out.emplace_back(scene->mMeshes[i]->mName.C_Str());
        }

        auto it = std::unique(out.begin(), out.end());
        out.resize(std::distance(out.begin(),it));

        return out;
    }
private:
    std::vector<u32> indices;
    std::vector<VertexFormat> rawVertexData;

    Assimp::Importer importer;
    const aiScene* scene { nullptr };

    // ! must be called before copying mesh data
    void copyIndices(aiMesh& mesh, std::vector<u32>& target, u32 vertexDataSize){
        u32 realSize = target.size();
        size_t indexStart = target.size();
        target.resize(target.size() + mesh.mNumFaces*3);
        for(u32 i=0; i < mesh.mNumFaces; i++){
            for(u32 w=0; w < mesh.mFaces[i].mNumIndices; w++){
                if(mesh.mFaces[i].mNumIndices != 3) continue;
                // indices are for single mesh, they need to be altered by current buffer size
                target[indexStart++] = mesh.mFaces[i].mIndices[w] + vertexDataSize;
                realSize += mesh.mFaces[i].mNumIndices;
            }
        }
        target.resize(realSize);
    }

    std::function<float(const std::string&)> getLayer;
    // std::string getTextureName(aiMesh *mesh){
    //     u32 materialIndex = mesh->mMaterialIndex;
    //     auto &material = *(scene->mMaterials[materialIndex]);
    //     aiString path;
    //     u32 texIndex = 0;


    //     if(AI_SUCCESS != material.GetTexture(aiTextureType_DIFFUSE, texIndex, &path))
    //         return "";

    //     u32 count = material.GetTextureCount(aiTextureType_DIFFUSE);
    //     // log(mesh->mName.C_Str(), "has", count, "textures:", path.C_Str());

    //     return getName(path.C_Str());
    // }

    std::vector<aiMesh*> findSubMeshesByMaterial(const std::string &name){
        if(debug) info("sub mesh", name);
        std::vector<aiMesh*> out;
        for(int i=0; i<scene->mNumMeshes; i++){
            if(scene->mMeshes[i]->mName == aiString(name)){
                out.push_back(scene->mMeshes[i]);
            }
        }
        return out;
}
};
