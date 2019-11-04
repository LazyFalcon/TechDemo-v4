class ShadowTexturePool;

enum ShadowSize
{
    BigShadow,
    MediumShadow,
    SmallShadow
}

class ShadowHandle
{
public:
    std::array<float, 2> position;
    std::array<float, 2> size;
    int textureId;
    ShadowSize shadowSize;
    bool needsUpdate;

    void release() {
        returnTo->releaseResource(id);
    }

private:
    int id;
    ShadowTexturePool* returnTo;
};

class ShadowTexturePool
{
private:
    std::map<int, ShadowHandle> m_freeResources;
    std::map<int, ShadowHandle> m_allocatedResources;
    std::vector<Texture> m_shadowTextures;

public:
    // * -> {4,8} : dzielimy na 4 częsci, a resztę na po osiem
    void allocateResources(
        Texture tex,
        std::vector<ShadowSize>
            requestedSizes) { // * split leci w rzędach, warto więc by było równo, nie ma skomplikowanego algorytmu podziału
        int texId = m_shadowTextures.size();
        m_shadowTextures.push_back(tex);

        glm::vec2 freeRect(tex.width, tex.height);
        glm::vec2 startingPoint(0, 0);

        bool notFinished = true;
        size_t partition = 0;
        while(notFinished) {
            float size = freeRect.y / requestedSizes[std::min(partition, requestedSizes.size())];

            startingPoint += glm::vec2(size, 0); // * move right

            partition++;
            startingPoint += glm::vec2(0, size); // * move up
        }
    }

    std::optional<ShadowHandle> allocateResource() {
        if(m_freeResources.empty())
            return {};
        return
    }
    void releaseResource(int id) {
        m_freeResources[id] = m_allocatedResources[id];
        m_allocatedResources.remove(id);
    }
};