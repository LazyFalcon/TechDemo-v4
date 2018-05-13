#pragma once

struct Texture;

struct FBO
{
    int attachedTextures {0};
    bool hasColor {false};

    FBO& full();
    FBO& half();
    FBO& quarter();
    FBO& tex(Texture&);

    void operator()();
};
