#pragma once
#include "core.hpp"
#include "Texture.hpp"

const uint INVALID_TEXTURE = 16985;

class ShadowPool
{
private:
    std::list<uint> m_freeSingles;
    std::list<uint> m_freeCubes;
    std::array<uint, 2> m_size;
    uint m_numberOfCubemaps;
    uint m_singleTextureSize;
    Texture m_texture {};

public:
    uint textureWithShadows;

    ShadowPool() : m_size({7, 6}), m_numberOfCubemaps(3), m_singleTextureSize(1024) {}

    void initTexture() {
        m_texture = Texture(gl::TEXTURE_2D, gl::DEPTH_COMPONENT32F, m_size[0] * m_singleTextureSize,
                            m_size[1] * m_singleTextureSize, 0, gl::DEPTH_COMPONENT, gl::FLOAT, gl::LINEAR, 0);

        for(uint i = 0; i < m_numberOfCubemaps; ++i) { m_freeCubes.push_back(i); }
        for(uint i = m_numberOfCubemaps; i < m_size[0]; ++i) {
            for(uint j = 0; j < m_size[0]; ++j) m_freeSingles.push_back(i + j * m_size[0]);
        }
    }

    std::optional<uint> allocate(uint count) {
        switch(count) {
            case 1:
                if(m_freeSingles.size()) {
                    auto i = m_freeSingles.front();
                    m_freeSingles.pop_front();
                    return i;
                }

                return std::nullopt;
            case 6:
                if(m_freeCubes.size()) {
                    auto i = m_freeCubes.front();
                    m_freeCubes.pop_front();
                    return i;
                }

                return std::nullopt;
            default: return std::nullopt;
        }
    }
    void release(uint index, uint count) {
        if(index < m_numberOfCubemaps) {
            m_freeCubes.push_back(index);
        }
        else {
            m_freeSingles.push_back(index);
        }
    }
    glm::vec2 idxToTexCoords(uint idx) const {
        float x = idx % m_size[0];
        float y = idx / m_size[0];
        return glm::vec2(x * m_singleTextureSize, y * m_singleTextureSize);
    }
    auto avaliableResources() {
        return std::make_pair(m_freeSingles.size(), m_freeCubes.size());
    }

    void release() {
        // deallocate texture
    }
};
