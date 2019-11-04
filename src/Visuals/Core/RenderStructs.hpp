#pragma once

struct Point
{
    glm::vec4 pos;
};
struct SSDot
{
    glm::vec2 position;
    float size;
    u32 color;
};
struct LensFlarePosition
{
    glm::vec4 pos;
    u32 color;
};
struct PointLightSource
{
    glm::vec4 position;
    u32 color;
    float size;
    float brightness;
};
struct Point2D
{
    glm::vec2 pos;
};
struct Line
{
    glm::vec4 pos[2];
};
struct PlasmaProjectile
{
    glm::vec4 from;
    glm::vec4 to;
    u32 color;
};

struct SimpleModelPbr
{
    Mesh mesh;
    glm::mat4 transform;
};

// * glMultiDrawElementsIndirect is for objects with different attrib format
struct DrawElementsIndirectCommand
{
    uint count;
    uint primCount; // * Specifies the number of instances of the indexed geometry that should be drawn.
    uint firstIndex;
    uint
        baseVertex; // * Specifies a constant that should be added to each element of indices when chosing elements from the enabled vertex arrays.
    uint
        baseInstance; // * Specifies the base instance for use in fetching instanced vertex attributes. // attribs, like VBO
};

const int CommandArraySize = 512; // * up to 1024 on some platforms, as it's maximum UBO size in mat4 matrices
struct CommandArray
{
    VAO vao;
    std::array<int, CommandArraySize * 10> count;
    std::array<void*, CommandArraySize * 10> indices;
    std::array<glm::mat4, CommandArraySize * 10> transforms;
    int size {};
    void push(int c, void* offset, const glm::mat4& mat) {
        count[size] = c;
        indices[size] = offset;
        transforms[size] = mat;
        ++size;
    }
    void clear() {
        size = 0;
    }
};
