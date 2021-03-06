#pragma once
#include <array>

struct Texture;
class FboHolder;
enum
{
    FULL,
    BY2,
    BY4,
    BY8,
    HALF_WIDE,
    SHADOWMAP,
    FBO_TYPES_END
};

class FBO
{
public:
    unsigned int id {};
    FboHolder* state;

    bool hasColor {true};

    void viewport(int x, int y, int z, int w);
    FBO& tex(Texture&);
    FBO& tex(Texture&, int);
    FBO& tex(int, int);
    FBO& operator()();

private:
    int m_attachedTextures {0};
    glm::ivec4 m_viewport {0, 0, 1, 1};
};

class FboHolder
{
public:
    bool colorDisabled;

    FboHolder(glm::vec2 screenSize) : m_screenSize(screenSize) {
        for(auto& it : m_fbos) { it.state = this; }
    }
    FBO& operator[](int i);
    FBO& current() {
        return *m_current;
    }
    void unbind();
    unsigned int drawBuffers[5];
    FBO& tex(Texture& t) {
        return m_current->tex(t);
    }
    FBO& tex(Texture& t, int i) {
        return m_current->tex(t, i);
    }
    FBO& tex(int t, int i) {
        return m_current->tex(t, i);
    }

private:
    glm::vec2 m_screenSize;
    std::array<FBO, FBO_TYPES_END> m_fbos {};
    unsigned int m_currentFboId {FBO_TYPES_END};
    FBO* m_current {nullptr};
};
