#pragma once
#include "gl_core_4_5.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "GPUResources.hpp"
#include "Logging.hpp"
#include "PhysicalWorld.hpp"
#include "Yaml.hpp"
#include "VertexData.hpp"

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
            // error(mesh.mName.C_Str(), " UV data not definied");

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
    std::string file;

    std::optional<Yaml> materials;
    // std::optional<std::reference_wrapper<>> detailTextures;

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
        file = filename;
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
            copyVertexData(*submodel, rawVertexData, subinfo.vertexStart);
            copyMaterial(*submodel, rawVertexData, subinfo.vertexStart);

            info.indexCount = indices.size() - info.indexStart;
            info.vertexCount = rawVertexData.size() - info.vertexStart;
            // if(getLayer) setTextureLayer(subinfo, getLayer(getTextureName(mesh)));
        }
        return info;
    }
    void copyVertexData(aiMesh& mesh, std::vector<VertexFormat>& data, int startPosition){
        vertexCopy::positions(mesh, data, startPosition);
        if constexpr(has_normal<VertexFormat>::value) vertexCopy::normals(mesh, data, startPosition);
        vertexCopy::texcoords(mesh, data, startPosition);
    }

    void copyMaterial(aiMesh& mesh, std::vector<VertexFormat>& data, int startPosition){
        if constexpr(has_color<VertexFormat>::value){
            if(not materials){
                error(file, "material struture not definied");
                return;
            }

            auto materialId = mesh.mMaterialIndex;
            if(scene->mNumMaterials <= materialId){
                error("For", mesh.mName.C_Str(), file, "scene has no material with id:", materialId);
                return;
            }

            auto& material = *scene->mMaterials[materialId];
            aiString aiMaterialName;
            material.Get(AI_MATKEY_NAME, aiMaterialName);
            auto& materialData = (*materials)[std::string(aiMaterialName.C_Str())];

            // if(aiColor3D aicolor(0.f,0.f,0.f); AI_SUCCESS == material.Get(AI_MATKEY_COLOR_DIFFUSE, aicolor)){
            //     glm::vec3 color(aicolor.r, aicolor.g, aicolor.b);
            // }
            // else {
            //     error("For", mesh.mName.C_Str(), "has no color in material");
            // }

            vertexCopy::color(mesh, data, startPosition, materialData["BaseColor"].vec3());
            float metallic = materialData["Metallic"].number();
            float roughness = materialData["Roughness"].number();
            float specular = materialData["Specular"].number();
            float anisotropic = materialData["Anisotropic"].number();
            // float anisotropicRotation = materialData["AnisotropicRotation"].number(); // temporary constant
            float clearcoat = materialData["Clearcoat"].number();
            // todo: add emissive materials
            float emissive = materialData["Emissive"].number();

            for(int i=startPosition; i<startPosition+mesh.mNumVertices; i++){
                data[i].roughness = roughness;
                data[i].metallic = metallic;
                data[i].specular = specular;
                data[i].anisotropic = anisotropic;
                data[i].clearcoat = clearcoat;
                data[i].emissive = emissive;
            }
        }
    }

    std::vector<ConvexMesh> loadConvexMeshes(const std::vector<std::string> &modelsToExtract){
        std::vector<ConvexMesh> out;
        for(auto& name : modelsToExtract){
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
                // TODO: fill position properly
                out.emplace_back(ConvexMesh{std::move(data), btVector3(0,0,0)});
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
        auto size = sizeof(VertexFormat);
        auto& vbo = vao.setup().addBuffer(rawVertexData.data(), rawVertexData.size()*size);
        vbo.attrib(0).pointer_float(3, size, (void*)offsetof(VertexFormat, position)).divisor(0);
        if constexpr(has_normal<VertexFormat>::value){
            vbo.attrib().pointer_float(3, size, (void*)offsetof(VertexFormat, normal)).divisor(0);
        }
        vbo.attrib().pointer_float(3, size, (void*)offsetof(VertexFormat, uv)).divisor(0);
        if constexpr(has_boneId<VertexFormat>::value){
            vbo.attrib().pointer_integer(1, size, (void*)offsetof(VertexFormat, boneId)).divisor(0);
        }
        if constexpr(has_color<VertexFormat>::value){
            vbo.attrib().pointer_float(3, size, (void*)offsetof(VertexFormat, color)).divisor(0);
            vbo.attrib().pointer_float(1, size, (void*)offsetof(VertexFormat, roughness)).divisor(0);
            vbo.attrib().pointer_float(1, size, (void*)offsetof(VertexFormat, metallic)).divisor(0);
            vbo.attrib().pointer_float(1, size, (void*)offsetof(VertexFormat, specular)).divisor(0);
            vbo.attrib().pointer_float(1, size, (void*)offsetof(VertexFormat, anisotropic)).divisor(0);
            vbo.attrib().pointer_float(1, size, (void*)offsetof(VertexFormat, clearcoat)).divisor(0);
            vbo.attrib().pointer_float(1, size, (void*)offsetof(VertexFormat, emissive)).divisor(0);
        }
        vao.addBuffer(indices)();
        vao.unbind();
        checkErrors();
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
