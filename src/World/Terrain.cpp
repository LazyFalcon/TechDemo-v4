#include "simplexnoise.h"
#include "gl_core_4_5.hpp"
#include "Logging.hpp"
#include "Terrain.hpp"
#include "QuadTree.hpp"
#include "PhysicsWorld.hpp"
#include "PerfTimers.hpp"

#include "Utils.hpp"
#include "ImageLoader.hpp"
#include "Yaml.hpp"

void Map::readDescription(const Yaml &cfg){
    size = cfg["WorldSize"].vec2();
    nodes = cfg["Nodes"].vec2();
    min = cfg["Min"].number();
    max = cfg["Max"].number();
    seed  = cfg["Seed"].as<u32>();
    units = size/(nodes-glm::vec2(1));
}

void Map::decode(i8 *mapData, u32 size){
    data.insert(data.end(), (float*)mapData, (float*)mapData + size/4);
}
void Map::encode(std::ofstream &stream){
    // ENCODE(size, stream);
    // ENCODE(nodes, stream);
    // ENCODE(min, stream);
    // ENCODE(max, stream);
    // auto dataSize = data.size();
    // ENCODE(dataSize, stream);
    // stream.write(reinterpret_cast<i8*>(data.data()), data.size()*sizeof(float));
}

Terrain::Terrain(QuadTree &qt) : qt(qt), compound(new btCompoundShape()){
    physicHeightData.resize(chunkNodes*chunkNodes * qt.lod0Count());
    bulletDataIterator = physicHeightData.data();
}

void Terrain::create(const Yaml &cfg){
    CPU_SCOPE_TIMER("Terrain::create");

    map.seed = cfg["Seed"].as<u32>();
    map.readDescription(cfg["Params"]);
    if(isFile(cfg["Dir"].string() + "/map.png")){
        if(cfg["Generation"].string() == "LoadFromFile" or cfg["Seed"].as<u32>() == cfg["Params"]["Seed"].as<u32>()){
            log("Terrain: load existing map");
            load(cfg["Dir"].string());
        }
        else if(cfg["Generation"].string() == "Random"){
            log("Terrain: create new map");
            generateRandom(cfg, map.seed, cfg["HeightRange"].number());
            save(cfg["Dir"].string());
        }
    }
    else {
        log("Terrain: file not found, create new map");
        generateRandom(cfg, map.seed, cfg["HeightRange"].number());
        save(cfg["Dir"].string());
    }
}

bool Terrain::load(const std::string &dir){
    CPU_SCOPE_TIMER("Terrain::load");
    log("min", map.min);
    log("max", map.max);
    auto image = ImageUtils::loadToMemory(dir+"/map.png", ImageDataType::R16);
    map.data.resize(map.noOfNodes());
    // std::transform((u16*)image.data, (u16*)image.data+image.dataSize/2, map.data.begin(), [this](u16 x){
    //     return (map.max-map.min)*x/float(0xffff) + map.min;
    // });

    for(auto y=0; y<image.height; y++){
        auto *bits = (u16*)image.data + y*image.width;
        for(auto x=0; x<image.width; x++){
            auto h = *(bits + x);
            map.data[x + y*image.width] = (map.max-map.min)*h/float(0xffff) + map.min;
            // *((u16*)image.data + x + y*image.width);
        }
    }
    image.clear();
    return true;
}
void Terrain::save(const std::string &dir){
    CPU_SCOPE_TIMER("Terrain::save");

    Yaml mapParams;
    mapParams["Nodes"] = map.nodes;
    mapParams["WorldSize"] = map.size;
    mapParams["Min"] = map.min;
    mapParams["Max"] = map.max;
    mapParams["Seed"] = map.seed;
    mapParams.save(dir+"/map.yml"s);

    std::vector<u16> packedMap(map.nodes.x*map.nodes.y);
    std::transform(map.data.begin(), map.data.end(), packedMap.begin(), [this](float h){
        return 0xffffu * ((h-map.min)/(map.max-map.min));
    });

    ImageUtils::ImageParams image {};
    image.width = map.nodes.x;
    image.height = map.nodes.y;
    image.dataType = ImageDataType::R16;
    image.data = (void*)packedMap.data();
    image.dataSize = map.data.size()*sizeof(u16);

    if(not ImageUtils::saveFromMemory(dir+"/map.png", ImageDataType::R16, image)){
        error("Unable to save map");
    }
}
void Terrain::generateRandom(const Yaml &cfg, u32 seed, double range){
    map.size = glm::vec2(cfg["Params"]["WorldSize"].vec2());
    map.nodes = cfg["Params"]["Nodes"].vec2();

    u32 newSize = map.noOfNodes();
    map.data.resize(newSize, 0);

    generator = [seed, range](glm::vec2 p){
        float h = 0;

        auto x = p.x + seed;
        auto y = p.y + seed;

        h += pow(scaled_octave_noise_2d(4, 0.4, 0.8, 0, 0.01, x/2.0, y/2.0), 2)*range;
        h += (scaled_octave_noise_2d(4, 0.4, 0.8, 0, range, 2000+x/1000.0, y/1000.0));
        h += (scaled_octave_noise_2d(3, 0.4, 0.8, 0, range*0.3, x/4000.0, y/4000.0));
        h += (scaled_octave_noise_2d(8, 0.4, 0.8, 0, 1, 236+x/100.0, 630+y/100.0))*range*0.1;

        return h;
    };

    for(u32 i=0; i<newSize; i++){
        auto p = map.toWorld({i % u32(map.nodes.x), i / map.nodes.y});

        map.data[i] = generator(p);
        map.min = std::min(map.min, map.data[i]);
        map.max = std::max(map.max, map.data[i]);
    }
}

void Terrain::uploadTexture(){
    if(glTextureID) deleteTexture();

    gl::GenTextures(1, &glTextureID);
    gl::BindTexture(gl::TEXTURE_2D, glTextureID);

    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::MIRRORED_REPEAT);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::MIRRORED_REPEAT);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
    gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST);

    gl::TexImage2D(gl::TEXTURE_2D, 0, gl::R32F, map.nodes.x, map.nodes.y, 0, gl::RED, gl::FLOAT, map.data.data());

    gl::BindTexture(gl::TEXTURE_2D, 0);
}
void Terrain::deleteTexture(){
    gl::DeleteTextures(1, &glTextureID);
    glTextureID = 0;
}

void Terrain::copyElevationData(QTNode &node){
    // convert from world xy to texture position
    u32 x = (node.center.x-node.dimensions.x*0.5f + map.size.x*0.5f)/map.size.x*(map.nodes.x-1);
    u32 y = (node.center.y-node.dimensions.y*0.5f + map.size.y*0.5f)/map.size.y*(map.nodes.y-1);

    for(u32 v=y; v<y+chunkNodes; v++){
        bulletDataIterator = std::copy(
                map.data.begin() + v*map.nodes.x + x,
                map.data.begin() + v*map.nodes.y + x + chunkNodes,
                bulletDataIterator
            );
    }
}

void Terrain::generatePayload(QTNode &node, PhysicsWorld &physics){
    if(not node.payload.terrainData){
        node.payload.terrainData = std::make_unique<TerrainData>();
    }
    auto &chunk = *(node.payload.terrainData);
    chunk.data = bulletDataIterator;
    copyElevationData(node);

    auto minMax = std::minmax_element(chunk.data, chunk.data+chunkNodes*chunkNodes);
    chunk.min = *minMax.first;
    chunk.max = *minMax.second;
    chunk.size = chunkNodes;
    node.center.z = chunk.min + (chunk.max - chunk.min)/2.f;
    chunk.heightOffset = 0;

    // physics.buildTerrainChunk(chunk.data, chunkNodes, node, compound);
}

void Terrain::finalize(){
    compound->recalculateLocalAabb();

    btTransform localTrans;
    localTrans.setIdentity();
    rgBody = qt.physics.createRigidBody(0, localTrans, compound, nullptr, 1);
    // rgBody->setCollisionFlags(rgBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    // whyyy???
    // pmk::_terrain = rgBody;
    // pmk::rgTerrain = rgBody;
}

std::vector<glm::vec4> Terrain::generateListOfVisibleNodes(){
    std::vector<glm::vec4> out;
    out.reserve(1000);

    for(auto &it : qt.lodLevels){
        out.insert(
                out.end(), it.renderData.begin(), it.renderData.end()
            );
    }
    return out;
}

std::vector<glm::vec4> Terrain::getNodesWithLod(u32 lod){
    std::vector<glm::vec4> out;
    out.reserve(pow(4, qt.levels+1-lod));

    for(auto &node : qt.QTNodes){
        if(node.lodLevel == lod)
            out.emplace_back(node.center.xyz(), node.dimensions.x);
    }

    return out;
}

float Terrain::sample(glm::vec2){return 0;}
