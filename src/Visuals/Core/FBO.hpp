#pragma once
#include <array>

struct Texture;
struct Window;

enum
{
    FULL = 1,
    HALF = 2,
    QUARTER = 4,
    HALF_WIDE = 12,
    SHADOWMAP = 22
};

class FBO
{
private:
    FBO& setLayer(unsigned int);

public:
    int attachedTextures {0};
    bool hasColor {false};
    std::array<float, 2> viewport {1, 1};
    Window& m_window;

    unsigned int drawBuffers[5];
    unsigned int shadowmap;
    unsigned int full;
    unsigned int _12;
    unsigned int _12_wide;
    unsigned int _14;
    unsigned int _18;
    unsigned int currentFbo {0};

    FBO(Window& window) : m_window(window) {}

    // select layer
    FBO& operator[](int);

    // attach texture
    FBO& tex(Texture&);

    // finalize and cleanup state
    FBO& operator()();
    FBO& setupDefaults();
};
