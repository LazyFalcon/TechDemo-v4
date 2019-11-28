#pragma once
#include "core.hpp"

class ShadowPool
{
private:
    std::list<uint> m_freeIndexes;
    // std::list<void*> m_allocatedCubes;
    // std::list<void*> m_allocatedSingle;
    uint m_parts;
    uint m_textureSize;

public:
    uint textureWithShadows;

    ShadowPool() : m_parts(16), m_textureSize(1024 * m_parts) {
        for(uint i = 0; i < m_parts * m_parts; i++) { m_freeIndexes.push_back(i); }
    }
    std::optional<std::array<uint, 6>> allocate(uint count /*, void* handle,  callbackToFreeResources*/) {
        /*
        switch(count) {
            case 1:
                if(m_freeIndexes.empty()) {
                    if(m_allocatedSingle.empty()) {
                        auto result =
                            callbackToFreeResources(m_allocatedSingle.extract(m_allocatedSingle.begin()).value());
                        m_allocatedSingle.insert(handle);
                        m_freeIndexes.insert(result.begin() + 1, result.end());
                        return result;
                    }
                    else if(m_allocatedCubes.empty()) {
                        return std::nullopt;
                    }
                    else {
                        auto result =
                            callbackToFreeResources(m_allocatedCubes.extract(m_allocatedCubes.begin()).value());
                        m_allocatedSingle.insert(handle);
                        return result;
                    }
                }
                break;
            case 6:
                if(m_freeIndexes.empty()) {
                    if(m_allocatedCubes.empty() and m_allocatedSingle.size() >= 6) {
                        std::array<uint, 6> result;
                        for(auto& it : result) {
                            it = callbackToFreeResources(
                                m_allocatedSingle.extract(m_allocatedSingle.begin()).value())[0];
                        }

                        m_allocatedCubes.insert(handle);
                        return result;
                    }
                    else if(m_allocatedSingle.size() < 6) {
                        return std::nullopt;
                    }
                    else {
                        auto result =
                            callbackToFreeResources(m_allocatedCubes.extract(m_allocatedCubes.begin()).value());
                        m_allocatedCubes.insert(handle);
                        return result;
                    }
                }
                break;
        }
        */

        if(m_freeIndexes.size() < count)
            return std::nullopt;

        std::array<uint, 6> result;
        for(int i = 0; i < count; ++i) {
            result[i] = m_freeIndexes.front();
            m_freeIndexes.pop_front();
        }
        return result;
    }
    void release(std::array<uint, 6>& indexes, uint count) {
        for(int i = 0; i < count; ++i) { m_freeIndexes.push_back(indexes[i]); }
    }
    glm::vec2 idxToTexCoords(uint idx) const {
        float x = idx % m_parts;
        float y = idx / m_parts;
        return glm::vec2(x, y);
    }
    uint avaliableResources() {
        return m_freeIndexes.size();
    }

    void init() {
        // allocate texture
        // hmm, może by jakiś wrapper na allokacjetekstur zrobić? tak żeby texture nie miało/potrzebowało dostepu do opengla?
    }
    void release() {
        // deallocate texture
    }
};
