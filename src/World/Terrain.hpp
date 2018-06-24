#pragma once

class QTNode;
class QuadTree;
 class PhysicalWorld;
class Yaml;

// TODO: rename
struct Map
{
    glm::vec2 size; // world size
    glm::vec2 nodes; // no of nodes
    glm::vec2 units; // no of nodes
    float min;
    float max;
    u32 seed;
    std::vector<float> data;
    u32 noOfNodes(){
        return nodes.x * nodes.y;
    }
    void readDescription(const Yaml&);
    void decode(i8 *data, u32 size);
    void encode(std::ofstream &stream);
    glm::vec2 toWorld(glm::vec2 p){
        return p*units - size*0.5f;
    }
};


class Terrain
{
public:
    Terrain(QuadTree &qt);
    ~Terrain(){}

    u32 chunkNodes {33/*global const, TODO: change later*/}; // no of vertices on chunk side

    u32 glTextureID {0};
    u32 glAtlas {0};
    u32 glNormalAtlas {0};

    std::string name;

    void create(const Yaml &cfg);
    void finalize();

    void generatePayload(QTNode &node, PhysicalWorld &physics);
    void uploadTexture();
    float sample(glm::vec2);
    std::vector<glm::vec4> generateListOfVisibleNodes();
    std::vector<glm::vec4> getNodesWithLod(u32 lod);
    std::function<float(glm::vec2)> generator;
protected:
    virtual void generateRandom(const Yaml &cfg, u32 seed, double range);
    bool load(const std::string &name);
    void save(const std::string &name);

    QuadTree &qt;
    btCompoundShape* compound {nullptr};
    btRigidBody* rgBody {nullptr};
    Map map {};
    std::vector<float> physicHeightData;
    float *bulletDataIterator;
    void deleteTexture();
    void copyElevationData(QTNode &node);
};
