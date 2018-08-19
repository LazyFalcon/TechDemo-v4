#pragma once
#include <type_traits>

#define HAS_MEMBER(x)\
    template<typename T, typename = void>\
    struct has_##x : std::false_type { };\
    template<typename T>\
    struct has_##x<T, decltype(T::x, void())> : std::true_type { };

HAS_MEMBER(normal)
HAS_MEMBER(tangent)
HAS_MEMBER(color)
HAS_MEMBER(boneId)

#undef HAS_MEMBER

struct VertexSimpleFlat
{
    glm::vec3 position;
    glm::vec3 uv;
};

struct VertexSimple
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 uv;
};

struct VertexWithMaterialData
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 uv;
    glm::vec3 color;
    float roughness;
    float metallic;
};

struct VertexWithMaterialDataAndBones
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 uv;
    uint boneId;
    glm::vec3 color;
    float roughness;
    float metallic;
};
