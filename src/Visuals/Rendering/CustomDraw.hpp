#pragma once
#include "GPUResources.hpp"
// #include "ModelLoader.hpp"

namespace graphic
{

// rename
struct CustomDrawMesh
{
    glm::mat4 tr;
    Mesh mesh;
    float layer;
    union {
        u32 color;
        u32 textureId;
    };
};

class CustomDraw
{
public:
    enum {
        DepthTest = 0x1,
        DepthMask = 0x2,
        Texture = 0x4,
        TextureArrayLayer = 0x8,
    };
    CustomDraw(){}
    CustomDraw(const std::string &params);
    void loadFromFile(const std::string &filename);
    void finalize();

    CustomDraw& model(const std::string&);
    CustomDraw& shape(const std::string&);
    CustomDraw& position(glm::vec2 p);
    CustomDraw& position(glm::vec4 p);
    CustomDraw& rotation(float r);
    CustomDraw& scale(float s);
    CustomDraw& scale(glm::vec3 s);
    CustomDraw& size(glm::vec2 p); // in pixels
    CustomDraw& transform(const glm::mat4 &tr);
    CustomDraw& detptMask();
    CustomDraw& detptTest();
    CustomDraw& texture(const std::string&);
    CustomDraw& texture(u32 id);
    CustomDraw& textureArray(u32 id, float layer);
    CustomDraw& color(u32 c);
    CustomDraw& operator ()();

    void setProjection(const glm::mat4 &tr){
        defaultProjection = tr;
    }

    static void render();
private:
    // ModelLoader loader;
    VAO vao {};
    u32 defaultParams {};
    u32 collectedParams {};
    CustomDrawMesh collectedDrawMesh {};
    glm::mat4 defaultProjection;

    static std::map<u32, std::map<u32, std::vector<CustomDrawMesh>>> renderQueues;
    std::map<std::string, Mesh> models;
};



}
