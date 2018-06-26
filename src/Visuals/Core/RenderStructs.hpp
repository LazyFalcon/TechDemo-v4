#pragma once

struct Point { glm::vec4 pos; };
struct SSDot { glm::vec2 position; float size; u32 color;};
struct LensFlarePosition { glm::vec4 pos; u32 color;};
struct PointLightSource {
    glm::vec4 position;
    u32 color;
    float size;
    float brightness;
};
struct Point2D { glm::vec2 pos; };
struct Line { glm::vec4 pos[2]; };
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
