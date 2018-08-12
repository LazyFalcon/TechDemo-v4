#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/half_float.hpp>
#include "GPUResources.hpp"
// TODO: zaoszczedzic na includach!
#include "PhysicalWorld.hpp"

struct InternalMeshInfo
{
    u32 vID;
    u32 uvID;
    u32 nID;
    u32 tID;
    u32 iID;

    u32 vertexCount;
    u32 count;

    Mesh toMesh(){
        return {iID, iID+count, count};
    }
};

struct InternalMesh
{
    InternalMesh() = default;
    InternalMesh(InternalMesh&&) = default;
    floats vertex;
    floats texcoord;
    floats normal;
    floats tangent;
    uints indices;

    void join(InternalMesh&);
};

struct VertexSimple
{
    glm::vec3 positon;
    glm::vec3 normal;
    glm::vec3 uv;
}

struct VertexWithMaterialData
{
    glm::vec3 positon;
    glm::vec3 normal;
    glm::vec3 uv;
    glm::vec3 color;
};

struct VertexWithMaterialDataAndBones
{
    glm::vec3 positon;
    glm::vec3 normal;
    glm::vec3 uv;
    int boneId;
    glm::vec3 color;
};

/**
  *  next lod levels has suffix _lod<level[1:3]>
  *  loading inserts in collection, position in collecion is returned
  *  for loadAll models are loaded in order of LOD
  *
  *
*/
// template<typename VertexFormat>
class ModelLoader
{
public:
    // u32 m_uvSize {3};
    // float m_vertexW { 1.f };
    bool good { false };
    // bool loadTangents { false };
    // bool loadUV { true };
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
        defaults.vertex4comonent = 1;
        defaults.uvComponents = 3;
    }

    ModelLoader& open(const std::string &filename, std::function<float(const std::string&)> getLayer);
    ModelLoader& open(const std::string &filename);
    void close();
    ~ModelLoader(){
        if(scene) close();
    }

    Mesh beginMesh(){
        Mesh mesh {};
        mesh.begin = indices.size();
        return mesh;
    }
    void endMesh(Mesh &m){
        m.end = indices.size();
        m.count = m.end - m.begin;
    }

    InternalMeshInfo load(const std::string &name);
    InternalMeshInfo load(const std::vector<std::string> &names);
    InternalMesh getInternalMesh(const std::string &name);
    InternalMeshInfo insert(InternalMesh &intMesh);
    std::vector<ConvexMesh> loadCompoundMeshes(const std::vector<std::string> &name);
    std::vector<std::string> findInstances(const std::string &pattern);
    void setTextureLayer(InternalMeshInfo info, float layer);
    void setBoneIndex(InternalMeshInfo info, float index);
    VAO build();

    std::pair<std::vector<double>, std::vector<u32>> loadStatic3DMesh(const std::string &name);

    std::vector<std::string> getNames();
private:
    floats vertex;
    floats texcoord;
    floats normal;
    floats tangent;
    uints indices;

    Assimp::Importer importer;
    const aiScene* scene { nullptr };
    void copyVertices(aiMesh *mesh, floats &target);
    void copyTexcoords(aiMesh *mesh, floats &target);
    void copyNormals(aiMesh *mesh, floats &target);
    void copyTangents(aiMesh *mesh, floats &target);
    void copyIndices(aiMesh *mesh, uints &target, u32 offset);

    std::function<float(const std::string&)> getLayer;
    std::string getTextureName(aiMesh *mesh);

    std::vector<aiMesh*> find(const std::string &name);
};
