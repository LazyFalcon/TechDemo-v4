#include "core.hpp"
#include "QuadTree.hpp"
#include "Logger.hpp"
#include "PerfTimers.hpp"
#include "PerfCounter.hpp"
#include "Terrain.hpp"
#include "Utils.hpp"
#include "PhysicalWorld.hpp"
#include "Yaml.hpp"

void QuadTree::init(const Yaml &cfg){
    levels = cfg["QtLevels"].number();
    size = cfg["Params"]["WorldSize"].vec2();
    nodes = glm::ivec2(cfg["Params"]["Nodes"].vec2());

    chunkSize = size/(float)pow(2, levels);
    qtNodeIndex = 0;

    int numberOfChunksInAllLevels = 0;
    for(int i = 0; i <= levels; i++){
        numberOfChunksInAllLevels += pow(4,i);
    }

    // trochę chyba le robię :/
    // niby jest ilosc leveli qt(i to chyba jest ok)
    // ale chyba lepiej by było jakoś inaczej to przeliczac, tak by qt dostosowywało swoj rozmiar inteligentnie do danych
    // dany jest wymiar mapy, ilosc wierzcholkow i ilosc wierzcholkow w chunku
    //

    console.info("QT", "nodes:", nodes);
    console.info("QT", "size:", size);
    console.info("QT", "chunk size:", chunkSize);
    console.info("QT", "node distance:", chunkSize/32.f);
    QTNodes.resize(numberOfChunksInAllLevels);

    QTNodes[0].center = glm::vec4(0,0,0,1);
    QTNodes[0].dimensions = glm::vec4(size, 0, 0);
    QTNodes[0].id = 0;
    QTNodes[0].parentId = 0;

    for(int i=0; i<numberOfChunksInAllLevels; i++){
        QTNodes[i].id = i;
        QTNodes[i].gridSize = 33; // TODO: hardcoded, num of vertices in graphic
    }
}

void QuadTree::buildQTNodes(Terrain &terrain){
    buildQTNodes(QTNodes[0], levels, terrain);
}
void QuadTree::buildQTNodes(QTNode &node, u32 lodLevel, Terrain &terrain){
    node.lodLevel = lodLevel;

    if(lodLevel == 0){ // final node
        buildQTLeaf(node, terrain);
    }
    else { // set child dimensions

        node.children[0] = &QTNodes[++qtNodeIndex];
        node.children[1] = &QTNodes[++qtNodeIndex];
        node.children[2] = &QTNodes[++qtNodeIndex];
        node.children[3] = &QTNodes[++qtNodeIndex];
        node.children[0]->parentId = node.id;
        node.children[1]->parentId = node.id;
        node.children[2]->parentId = node.id;
        node.children[3]->parentId = node.id;

        node.children[0]->center = node.center + node.dimensions*glm::vec4(0.25, -0.25,0,0);
        node.children[1]->center = node.center + node.dimensions*glm::vec4(0.25, 0.25,0,0);
        node.children[2]->center = node.center + node.dimensions*glm::vec4(-0.25, 0.25,0,0);
        node.children[3]->center = node.center + node.dimensions*glm::vec4(-0.25, -0.25,0,0);

        node.children[0]->dimensions = node.dimensions*glm::vec4(0.5, 0.5, 0, 0);
        node.children[1]->dimensions = node.dimensions*glm::vec4(0.5, 0.5, 0, 0);
        node.children[2]->dimensions = node.dimensions*glm::vec4(0.5, 0.5, 0, 0);
        node.children[3]->dimensions = node.dimensions*glm::vec4(0.5, 0.5, 0, 0);

        node.children[0]->parent = &node;
        node.children[1]->parent = &node;
        node.children[2]->parent = &node;
        node.children[3]->parent = &node;

        node.center.z = 0;
        node.dimensions.z = 0;

        buildQTNodes(*node.children[0], lodLevel-1, terrain);
        buildQTNodes(*node.children[1], lodLevel-1, terrain);
        buildQTNodes(*node.children[2], lodLevel-1, terrain);
        buildQTNodes(*node.children[3], lodLevel-1, terrain);

        node.center.z = (node.children[0]->center.z + node.children[1]->center.z + node.children[2]->center.z + node.children[3]->center.z)/4.f;
    }
}

/**
 * @brief Calculates node z values, generates physical data, builds bullet rgBody(via terrain)
 */
void QuadTree::buildQTLeaf(QTNode &node, Terrain &terrain){
    node.children = { nullptr, nullptr, nullptr, nullptr };

    i32 x = (node.center.x-node.dimensions.x*0.5f + size.x/2.f)/1.f;
    i32 y = (node.center.y-node.dimensions.y*0.5f + size.y/2.f)/1.f;

    terrain.generatePayload(node, physics);

}
void QuadTree::recalculateNodeZPosition(){
    CPU_SCOPE_TIMER("QuadTree::recalculateNodeZPosition");
    recalculateNodeZPosition(QTNodes[0]);
}
std::pair<float, float> QuadTree::recalculateNodeZPosition(QTNode &node){
    if(node.children[0]){
        auto a = recalculateNodeZPosition(*node.children[0]);
        auto b = recalculateNodeZPosition(*node.children[1]);
        auto c = recalculateNodeZPosition(*node.children[2]);
        auto d = recalculateNodeZPosition(*node.children[3]);

        node.min = std::min({a.first, b.first, c.first, d.first});
        node.max = std::min({a.second, b.second, c.second, d.second});
    }
    else {
        node.min = node.payload.terrainData->min;
        node.max = node.payload.terrainData->max;
        node.center.z = node.min + (node.max - node.min)/2.f;
    }

    // node.center.z = node.min + (node.max - node.min)*0.5f;
    node.dimensions.z = node.max - node.min;
    return {node.min, node.max};
}
void QuadTree::registerInBullet(QTNode &node){}

// ------ CULLING ------
typedef std::vector<QTNode*> nodeVec;
void diff(nodeVec &old, nodeVec &cur, nodeVec &added, nodeVec &removed){
    std::sort(old.begin(), old.end());
    std::sort(cur.begin(), cur.end());

    u32 oldS = old.size();
    u32 curS = cur.size();
    u32 oldI = 0;
    u32 curI = 0;
    for(; oldI<oldS && curI<curS;){
        if(old[oldI] == cur[curI]){
            oldI++;
            curI++;
        }
        else if(old[oldI] < cur[curI]){
            removed.push_back(old[oldI]);
            oldI++;
        }
        else {
            added.push_back(cur[curI]);
            curI++;
        }
    }
    for(; oldI<oldS; oldI++){
        removed.push_back(old[oldI]);
    }
    for(; curI<curS; curI++){
        added.push_back(cur[curI]);
    }
}

void QuadTree::update(const camera::Frustum &frustum){
    CPU_SCOPE_TIMER("QuadTree::update");
    std::array<std::vector<QTNode*>, 5> prevoiusFrame {};

    for(u32 i = 0; i<lodLevels.size(); i++){
        prevoiusFrame[i].swap(lodLevels[i].visibleNodes);
        lodLevels[i] = {};
    }
    frustumCull(QTNodes[0], frustum);

    PerfCounter::records["lod0 visible: "] = lodLevels[0].visibleNodes.size();
    for(u32 i = 0; i<lodLevels.size(); i++){
        auto &it = lodLevels[i];

        diff(prevoiusFrame[i], it.visibleNodes, it.addedNodes, it.removedNodes);
        for(auto &r : it.removedNodes){
            r->visible = false;
        }

        std::sort(it.renderData.begin(), it.renderData.end(), [&frustum](const glm::vec4 &a, const glm::vec4 &b)->bool{
            return glm::distance2(frustum.eye, a) > glm::distance2(frustum.eye, b);
        });
    }
}

LodLevel QuadTree::findLodLevel(QTNode &node, const camera::Frustum &frustum){
    float manhattanLodDistances[4] = {15,60,100,900};

    auto vec = glm::abs(node.center - glm::ceil(frustum.eye/chunkSize.x*10.f)*chunkSize.x/10.f);
    auto distance = std::max(vec.x, vec.y);
    // auto distance = abs(vec.x) + abs(vec.y);
    // auto distance = sqrt(vec.x*vec.x + vec.y*vec.y);

    if(node.lodLevel == 3)
        // return distance > manhattanLodDistances[3] ? LOD_3 : LOD_2;
        return distance > 26*chunkSize.x ? LOD_3 : LOD_2;
    if(node.lodLevel == 2)
        // return distance > manhattanLodDistances[2] ? LOD_2 : LOD_1;
        return distance > 18*chunkSize.x ? LOD_2 : LOD_1;
    if(node.lodLevel == 1)
        // return distance > manhattanLodDistances[1] ? LOD_1 : LOD_0;
        return distance > 9*chunkSize.x ? LOD_1 : LOD_0;
    if(node.lodLevel == 0)
        return distance <= 10*chunkSize.x ? LOD_0 : DUMMY;
        // return distance <= manhattanLodDistances[0] ? LOD_0 : DUMMY;

    return DUMMY;
}
bool nearestChunk(QTNode &node, glm::vec4 eye){
    return node.lodLevel == 0 and glm::distance(eye, node.center)<20.f;
}
/**
 * ueh, bleeh
 * co ma sie dziac? czy keszujemy zoccludowane; jak wykryc te ktore si usunely(diff vectorów<*>);
 * no i jest to rekurencyjne, pamietac zeby nie dodać rodzica jesli dziecko dodalismy!
 * > zrobione poprzez porównywanie levelu z wysokocia w hierarchi
 *
 */
void QuadTree::frustumCull(QTNode &node, const Frustum &frustum){
    if(enableOC && node.occluded){
        // TODO save this info
        return;
    }
    // if(true or frustum.testSphere(node.center, node.dimensions.x*0.9f)){
    if(frustum.testSphere(node.center, node.dimensions.x*0.9f)){
    // if(nearestChunk(node, frustum.eye) or frustum.testSphere(node.center, node.dimensions.x*0.9f)){
        auto level = findLodLevel(node, frustum);
        if(level == node.lodLevel){
            addToVisible(node, level);
            return;
        }
        else if(level == OUT_OF_VIEW){
            node.visible = false;
            return;
        }
    }
    if(not node.children[0]) return;
    frustumCull(*node.children[0], frustum);
    frustumCull(*node.children[1], frustum);
    frustumCull(*node.children[2], frustum);
    frustumCull(*node.children[3], frustum);
}
void QuadTree::addToVisible(QTNode &node, LodLevel level){
    lodLevels[level].visibleNodes.push_back(&node);
    lodLevels[level].renderData.push_back(glm::vec4(node.center.xyz(), node.dimensions.x));
    node.visible = true;
}
void QuadTree::removevisible(QTNode &node){}

bool QTNode::insideFlat(const glm::vec4 &position) const {
    auto diff = glm::abs(center - position);
    return diff.x <= dimensions.x*0.5f && diff.y <= dimensions.y*0.5f;
}
glm::vec4 QTNode::distanceTo(const glm::vec4 &position) const {
    auto diff = glm::abs(center - position);
    return diff;
}
glm::vec4 QTNode::distanceToBorders(const glm::vec4 &position) const {
    auto diff = glm::abs(center - position);
    return diff - dimensions;
}

QTNode& QuadTree::findTouchedNodes(glm::vec4 p, float radius){
    /**
        sprawdzamy czy nie dotknęliśmy dwojki z dzieci, jesli tak to zwracamy obecny,
        jesli dzieci nie ma to tez
    */
    QTNode *node = &QTNodes[0];
    for(u32 lv=0; lv<levels; lv++){
        if(not node->children[0]) return *node;
        std::vector<QTNode*> touchedChilds;
        for(u32 i=0; i<4; i++){
            auto borderDistance = node->children[i]->distanceToBorders(p);
            if(borderDistance.x < radius && borderDistance.y < radius)
                touchedChilds.push_back(node->children[i]);
        }
        if(touchedChilds.size() == 1)
            node = node->children[0];
        else
            return *node;
    }
    return *node;
}

// ------ UTILS ------
SampleResult QuadTree::sample(glm::vec4 position){
    btVector3 from(position.x, position.y, position.z + 500.f);
    btVector3 to(position.x, position.y, position.z - 500.f);

    btCollisionWorld::ClosestRayResultCallback    closestResults(from, to);
    physics.m_dynamicsWorld->rayTest(from, to, closestResults);

    if(closestResults.hasHit()){
        return {convert(closestResults.m_hitPointWorld, 1), convert(closestResults.m_hitNormalWorld, 0), true};
    }
    else {
        return {{0,0,-500,0}};
    }
}
SampleResult QuadTree::sampleTerrain(glm::vec4 position){
    btVector3 from(double(position.x), double(position.y), double(position.z + 1500.f));
    btVector3 to(double(position.x), double(position.y), double(position.z - 1500.f));

    btCollisionWorld::AllHitsRayResultCallback    allResults(from, to);
    physics.m_dynamicsWorld->rayTest(from, to, allResults);

    if(allResults.hasHit()){
        u32 minI=0;
        for(u32 i=0; i<allResults.m_hitPointWorld.size(); i++ ){
            if(allResults.m_hitPointWorld[i][2] < allResults.m_hitPointWorld[minI][2]) {
                minI = i;
            }
        }
        return {convert(allResults.m_hitPointWorld[minI], 1), convert(allResults.m_hitNormalWorld[minI], 0), true};
    }
    else {
        return {{0,0,-500,0}};
    }

}
SampleResult QuadTree::sampleHeight(glm::vec4 position, const QTNode *node) const {
    auto findChunk = [this, &position] ()->i32 {
        if(not QTNodes[0].insideFlat(position)){
            return -1;
        }
        const QTNode *node = &(QTNodes[0]);
        while(true){
            if(node->lodLevel == 0) return node->id;
            if(node->children[0]->insideFlat(position)) node = node->children[0];
            else if(node->children[1]->insideFlat(position)) node = node->children[1];
            else if(node->children[2]->insideFlat(position)) node = node->children[2];
            else if(node->children[3]->insideFlat(position)) node = node->children[3];
            else {
                return -1;
            }
        }
        return -1;
    };

    if(node->lodLevel != 0 or not node->insideFlat(position)){
        i32 id = findChunk();
        if(id == -1){
            return {{0,0,-500,0}};
        }
        node = &QTNodes[id];
    }
    return node->sampleHeight(position);
}
SampleResult QTNode::sampleHeight(glm::vec4 position) const {
    if(not insideFlat(position)) return {{0,0,-500,0}};
    glm::vec4 local = (position - center + dimensions*0.5f) / dimensions * (gridSize-1); // [0-33]
    double x_, y_;
    float u = modf(local.x, &x_);
    float v = modf(local.y, &y_);
    u32 x(x_), y(y_);

    position.z = pmk::triangleInterpolate(
            (*payload.terrainData)(x, y),
            (*payload.terrainData)(x, y+1),
            (*payload.terrainData)(x+1, y),
            (*payload.terrainData)(x+1, y+1),
            u,
            v);

    return {position, glm::vec4(pmk::triangleNormal(
            (*payload.terrainData)(x, y),
            (*payload.terrainData)(x, y+1),
            (*payload.terrainData)(x+1, y),
            (*payload.terrainData)(x+1, y+1),
            u,
            v, dimensions.x/gridSize),0), true};
}
