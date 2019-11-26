#pragma once

class ShadowPool
{
private:
    std::list<uint> m_freeIndees;
    uint m_parts;
    uint m_textureSize;

public:
    ShadowPool() : m_parts(16), m_textureSize(1024 * m_parts) {
        for(uint i = 0; i < m_parts * m_parts; i++) { m_freeIndees.push_back(i); }
    }
    std::optional<uint> acquire() {
        if(m_freeIndees.empty())
            return std::nullopt;

        int idx = m_freeIndees.front();
        m_freeIndees.pop_front();
        return idx;
    }
    void release(uint idx) {
        m_freeIndees.push_back(idx);
    }
    glm::vec2 idxToTexCoords(uint idx) const {
        float x = idx % m_parts;
        float y = idx / m_parts;
        return glm::vec2(x, y);
    }
    void init() {
        // allocate texture
        // hmm, może by jakiś wrapper na allokacjetekstur zrobić? tak żeby texture nie miało/potrzebowało dostepu do opengla?
    }
    void release() {
        // deallocate texture
    }
};
