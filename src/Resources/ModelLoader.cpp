#include "core.hpp"
#include "ModelLoader.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

// ! http://assimp.sourceforge.net/lib_html/structai_mesh.html

// std::vector<aiMesh*> ModelLoader::findSubMeshesByMaterial(const std::string &name){
//     if(debug) console.info("sub mesh", name);
//     std::vector<aiMesh*> out;
//     for(int i=0; i<scene->mNumMeshes; i++){
//         if(scene->mMeshes[i]->mName == aiString(name)){
//             out.push_back(scene->mMeshes[i]);
//         }
//     }
//     return out;
// }
//
// std::string ModelLoader::getTextureName(aiMesh *mesh){
//     u32 materialIndex = mesh->mMaterialIndex;
//     auto &material = *(scene->mMaterials[materialIndex]);
//     aiString path;
//     u32 texIndex = 0;


//     if(AI_SUCCESS != material.GetTexture(aiTextureType_DIFFUSE, texIndex, &path))
//         return "";

//     u32 count = material.GetTextureCount(aiTextureType_DIFFUSE);
//     // console.log(mesh->mName.C_Str(), "has", count, "textures:", path.C_Str());

//     return getName(path.C_Str());
// }
/*
void InternalMesh::join(InternalMesh &intMesh){
    u32 offset = vertex.size()/4;
    for(auto &it : intMesh.indices) it += offset;

    indices.insert(indices.end(), intMesh.indices.begin(), intMesh.indices.end());
    vertex.insert(vertex.end(), intMesh.vertex.begin(), intMesh.vertex.end());
    texcoord.insert(texcoord.end(), intMesh.texcoord.begin(), intMesh.texcoord.end());
    normal.insert(normal.end(), intMesh.normal.begin(), intMesh.normal.end());
    tangent.insert(tangent.end(), intMesh.tangent.begin(), intMesh.tangent.end());
}

void ModelLoader::copyVertices(aiMesh *mesh, floats &target){
    u32 count = mesh->mNumVertices;
    u32 start = target.size();
    target.resize(target.size()+count*4);
    for(u32 i=0, j=0; i<count; i++, j+=4){
        target[start+j] = mesh->mVertices[i].x;
        target[start+j+1] = mesh->mVertices[i].y;
        target[start+j+2] = mesh->mVertices[i].z;
        target[start+j+3] = defaults.vertex4comonent;
    }
}
// * always 3 coords
void ModelLoader::copyTexcoords(aiMesh *mesh, floats &target){
    u32 count = mesh->mNumVertices;
    u32 start = target.size();
    if(not defaults.uvComponents){
        count = mesh->mNumVertices;
        target.resize(target.size()+count * (*defaults.uvComponents));

        for(u32 i=0, j=0; i<count; i++, j+=2){
            target[start+j] = 0;
            target[start+j+1] = 0;
            target[start+j+2] = 0;
        }
        return;
    }

    target.resize(target.size()+count*(*defaults.uvComponents));
    if(not mesh->mTextureCoords[0]){
        if((*defaults.uvComponents) == 2u) for(u32 i=0, j=0; i<count; i++, j+=2){
            target[start+j] = 0;
            target[start+j+1] = 0;
        }
        if((*defaults.uvComponents) == 3u) for(u32 i=0, j=0; i<count; i++, j+=3){
            target[start+j] = 0;
            target[start+j+1] = 0;
            target[start+j+2] = 0;
        }
        return;
    }

    if((*defaults.uvComponents) == 2u) for(u32 i=0, j=0; i<count; i++, j+=2){
        target[start+j] = mesh->mTextureCoords[0][i].x;
        target[start+j+1] = mesh->mTextureCoords[0][i].y;
    }
    if((*defaults.uvComponents) == 3u) for(u32 i=0, j=0; i<count; i++, j+=3){
        target[start+j] = mesh->mTextureCoords[0][i].x;
        target[start+j+1] = mesh->mTextureCoords[0][i].y;
        target[start+j+2] = mesh->mTextureCoords[0][i].z;
    }
}
void ModelLoader::copyNormals(aiMesh *mesh, floats &target){
    u32 count = mesh->mNumVertices;
    u32 start = target.size();
    target.resize(target.size()+count*4);
    for(u32 i=0, j=0; i<count; i++, j+=4){
        target[start+j] = mesh->mNormals[i].x;
        target[start+j+1] = mesh->mNormals[i].y;
        target[start+j+2] = mesh->mNormals[i].z;
        target[start+j+3] = 0;
    }
}
void ModelLoader::copyTangents(aiMesh *mesh, floats &target){
    if(not defaults.tangents) return;
    u32 count = mesh->mNumVertices;
    u32 start = target.size();
    target.resize(target.size()+count*4);
    for(u32 i=0, j=0; i<count; i++, j+=4){
        target[start+j] = mesh->mTangents[i].x;
        target[start+j+1] = mesh->mTangents[i].y;
        target[start+j+2] = mesh->mTangents[i].z;
        target[start+j+3] = 0;
    }
}
void ModelLoader::copyIndices(aiMesh *mesh, uints &target, u32 offset){
    u32 count = mesh->mNumVertices;

    target.reserve(target.size()+count*4);

    for(u32 i=0; i < mesh->mNumFaces; i++){
        for(u32 w=0; w < mesh->mFaces[i].mNumIndices; w++){
            target.push_back(mesh->mFaces[i].mIndices[w] + offset);
        }
    }
}

ModelLoader& ModelLoader::open(const std::string &filename, std::function<float(const std::string&)> getLayerFunction){
    getLayer = getLayerFunction;
    return open(filename);
}
ModelLoader& ModelLoader::open(const std::string &filename){
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
    vertex.reserve(600000);
    texcoord.reserve(400000);
    normal.reserve(600000);
    if(defaults.tangents) tangent.reserve(600000);
    indices.reserve(600000);

    good = (bool)scene;
    return *this;
}
void ModelLoader::close(){
    importer.FreeScene();
    vertex.clear();
    texcoord.clear();
    normal.clear();
    tangent.clear();
    scene = nullptr;
}
VAO ModelLoader::build(){
    VAO vao {};
    vao.setup().addBuffer(vertex, 4);
        if(defaults.uvComponents) vao.addBuffer(texcoord, *defaults.uvComponents);
        vao.addBuffer(normal, 4);
        if(loadTangents) vao.addBuffer(tangent, 4);
        vao.addBuffer(indices)();
    return vao;
}

std::vector<ConvexMesh> ModelLoader::loadConvexMeshes(const std::vector<std::string> &names){
    std::vector<ConvexMesh> out;
    for(auto& name : names){
        auto meshes = find(name);
        if(meshes.empty()){
            console.error("no mesh");
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
std::vector<std::string> ModelLoader::findInstances(const std::string &pattern){
    std::vector<std::string> out;
    for(u32 i=0; i<scene->mNumMeshes; i++){
        aiMesh *mesh = scene->mMeshes[i];
        std::string meshName(mesh->mName.C_Str());
        if(std::string::npos != meshName.find(pattern)){
            out.emplace_back(meshName);
            // console.info("instance", meshName);
        }
    }
    return out;
}

/*
* loads specified mesh from file.
* there is only one diffuse texture per material, so models are split by materials, need to be combined
*/
/*
InternalMeshInfo ModelLoader::load(const std::string &name){
    if(not scene){
        console.error("No scene for ModelLoader");
        hardPause();
        return {};
    }
    InternalMeshInfo info {(u32)vertex.size(), (u32)texcoord.size(), (u32)normal.size(), (u32)tangent.size(), (u32)indices.size()};

    auto meshes = find(name);
    if(meshes.empty()){
        console.error("no mesh:", name);
        return {};
    }
    for(auto mesh : meshes){
        InternalMeshInfo subinfo {(u32)vertex.size(), (u32)texcoord.size(), (u32)normal.size(), (u32)tangent.size(), (u32)indices.size()};
        copyIndices(mesh, indices, vertex.size()/4);
        copyVertices(mesh, vertex);
        copyTexcoords(mesh, texcoord);
        copyNormals(mesh, normal);
        copyTangents(mesh, tangent);

        info.count = indices.size() - info.iID;
        info.vertexCount = (vertex.size() - info.vID)/4;
        subinfo.count = indices.size() - subinfo.iID;
        subinfo.vertexCount = (vertex.size() - subinfo.vID)/4;
        if(getLayer) setTextureLayer(subinfo, getLayer(getTextureName(mesh)));
    }
    return info;
}

InternalMeshInfo ModelLoader::load(const std::vector<std::string> &names){

    InternalMeshInfo info {(u32)vertex.size(), (u32)texcoord.size(), (u32)normal.size(), (u32)tangent.size(), (u32)indices.size()};

    std::vector<aiMesh*> meshes;

    for(auto& name : names){
        auto f = find(name);
        meshes.insert(meshes.end(), f.begin(), f.end());
    }

    if(meshes.empty()){
        console.error("no mesh:", names[0]);
        return {};
    }
    for(auto mesh : meshes){
        InternalMeshInfo subinfo {(u32)vertex.size(), (u32)texcoord.size(), (u32)normal.size(), (u32)tangent.size(), (u32)indices.size()};
        copyIndices(mesh, indices, vertex.size()/4);
        copyVertices(mesh, vertex);
        copyTexcoords(mesh, texcoord);
        copyNormals(mesh, normal);
        copyTangents(mesh, tangent);

        info.count = indices.size() - info.iID;
        info.vertexCount = (vertex.size() - info.vID)/4;
        subinfo.count = indices.size() - subinfo.iID;
        subinfo.vertexCount = (vertex.size() - subinfo.vID)/4;
        if(getLayer) setTextureLayer(subinfo, getLayer(getTextureName(mesh)));
    }
    return info;

    return {};
}

InternalMeshInfo ModelLoader::insert(InternalMesh &intMesh){
    InternalMeshInfo info {(u32)vertex.size(), (u32)texcoord.size(), (u32)normal.size(), (u32)tangent.size(), (u32)indices.size()};

    u32 offset = vertex.size()/4;
    for(auto &it : intMesh.indices) it += offset;

    indices.insert(indices.end(), intMesh.indices.begin(), intMesh.indices.end());
    vertex.insert(vertex.end(), intMesh.vertex.begin(), intMesh.vertex.end());
    if(defaults.uvComponents) texcoord.insert(vertex.end(), intMesh.texcoord.begin(), intMesh.texcoord.end());
    normal.insert(normal.end(), intMesh.normal.begin(), intMesh.normal.end());
    if(defaults.tangents) tangent.insert(tangent.end(), intMesh.tangent.begin(), intMesh.tangent.end());

    info.count = indices.size() - info.iID;
    info.vertexCount = (vertex.size() - info.vID)/4;

    return info;
}

InternalMesh ModelLoader::getInternalMesh(const std::string &name){
    if(not scene){
        console.error("No scene for ModelLoader");
        hardPause();
        return {};
    }
    InternalMesh intMesh {};

    auto meshes = find(name);
    if(meshes.empty()){
        console.error("no mesh:", name);
        return {};
    }
    for(auto mesh : meshes){
        copyIndices(mesh, intMesh.indices, intMesh.vertex.size()/4);
        copyVertices(mesh, intMesh.vertex);
        copyTexcoords(mesh, intMesh.texcoord);
        copyNormals(mesh, intMesh.normal);
        copyTangents(mesh, intMesh.tangent);

    }
    return intMesh;
}



void ModelLoader::setTextureLayer(InternalMeshInfo info, float layer){
    for(u32 i=0; i<info.vertexCount; i++){
        texcoord[info.uvID+i*3+2] += layer;
    }
}

void ModelLoader::setBoneIndex(InternalMeshInfo info, float index){
    for(u32 i=0; i<info.vertexCount; i++){
        vertex[info.vID+i*4+3] += index;
    }
}

std::vector<std::string> ModelLoader::getNames(){
    std::vector<std::string> out;

    for(int i=0; i<scene->mNumMeshes; i++){
        out.emplace_back(scene->mMeshes[i]->mName.C_Str());
    }

    auto it = std::unique(out.begin(), out.end());
    out.resize(std::distance(out.begin(),it));

    return out;
}

std::pair<std::vector<double>, std::vector<u32>> ModelLoader::loadStatic3DMesh(const std::string &name){
    std::vector<double> outVerts;
    std::vector<u32> outIndices;
    if(not scene){
        console.error("No scene for ModelLoader");
        hardPause();
        return {};
    }

    auto meshes = find(name);
    if(meshes.empty()){
        console.error("no mesh:", name);
        return {};
    }


    for(auto mesh : meshes){
        copyIndices(mesh, outIndices, outVerts.size()/4);
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
*/
