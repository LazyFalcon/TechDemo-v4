#pragma once
#include "GpuResources.hpp"

/*
*  Simple starfield generator
*/
union Color
{
    struct
    {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };
    u32 rgba;
};

struct Star
{
    glm::vec4 position;
    Color color;
    float brightness;
};
// moze lepiej starfield?
class Starfield
{
public:
    Starfield() {}
    void regenerate();
    void update();
    VAO upload(std::vector<Star>& stars);

    VAO starsOnGpu;
    u32 starCount {};
};
