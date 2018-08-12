#pragma once

struct Shader;
struct Image;
struct VAO;
struct Mesh;
struct ImageSet;
struct Font;

namespace assets {

struct TextureArray
{
    u32 id;
    u32 width;
    u32 height;
    std::vector<std::string> content;
    float find(const std::string &name){
        float out = 0.f;
        for(const auto &it : content){
            if(it == name) return out;
            out += 1.f;
        }
        return out;
    }
};

Image getImage(const std::string &name);
TextureArray& getAtlas(const std::string &name);
TextureArray& getAlbedoArray(const std::string &name);
TextureArray& getNormalArray(const std::string &name);
TextureArray& getMetalic(const std::string &name);
TextureArray& getRoughnessArray(const std::string &name);
TextureArray& getCubeMap(const std::string &name);
Shader& setShader(const std::string &name);
Shader& getShader(const std::string &name);
Shader& bindShader(const std::string &name);
Font& getFont(const std::string &name);

std::string findArrayWithTextureName(const std::string &name);
std::function<float(const std::string&)> layerSearch(TextureArray&);

void relaseResources(u32 id);

void addImageSet(ImageSet &imageSet, const std::string &name);
ImageSet& getImageSet(const std::string &name);
void addShader(Shader &shader, const std::string &name);

void addImage(u32 id, Image &image, const std::string &name);
void addAlbedoArray(u32 id, TextureArray &array, const std::string &name);
void addNormalArray(u32 id, TextureArray &array, const std::string &name);
void addMetallicArray(u32 id, TextureArray &array, const std::string &name);
void addRoughnessArray(u32 id, TextureArray &array, const std::string &name);
void addCubeMap(u32 id, TextureArray &array, const std::string &name);

void addVao(VAO vao, const std::string &name);
VAO& getVao(const std::string &name);

void addMesh(Mesh vao, const std::string &name);
Mesh& getMesh(const std::string &name);

};
