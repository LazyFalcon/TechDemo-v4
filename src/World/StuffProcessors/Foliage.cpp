#include "core.hpp"
#include "gl_core_4_5.hpp"
#include "Foliage.hpp"
#include "Colors.hpp"
#include "Constants.hpp"
#include "ModelLoader.hpp"
#include "PerfTimers.hpp"
#include "PhysicalWorld.hpp"
#include "ResourceLoader.hpp"
#include "Sampler2D.hpp"
#include "Utils.hpp"
#include "Yaml.hpp"
#include "simplexnoise.h"
// const u32 MAX_TREES_PER_NODE = 18u;
const u32 MAX_TREES_PER_NODE = 8u;
const u32 MAX_SHRUB_PER_NODE = 23u;

const float lodDistances[] = {80, 130, 200, 400, 1000};

btCollisionShape* treeCapsuleShape;
btCompoundShape* treeCapsuleCompoundShape;
std::function<void(btRigidBody*& body, btGeneric6DofSpring2Constraint* constraint)> RenderData::removeTreeFromPhysics;
std::vector<std::string> Foliage::prepareAtlas() {
    return listDirectory(resPath + "foliage/textures");
}

typedef std::set<QTNode*> QtChunks;
const float treeCOMOffset = 4.5;

Foliage::~Foliage() {}
// read all from ../res/foliage/[bush, grass, trees] to one VAO and one texture array
// NOTE: how to bind model with texture? do we need configureation file?
// TODO: ujednolicic zmienianie scierzek z zasobami
// TODO: zrefaktoryzowac zarzadzanie zasobami GPU
// TODO: obsluga bardziej skomplikowanych modeli? np drzewo to bedzie pien i kilka galezi
/**
Tree cfg format:
    TreeName:
        lod0:
            - Mesh: asdf
              Texture: asgszd
            - ...
        lod1: ...
unknown number of configs, unknow number of tree in config
one config per file
*/
bool Foliage::load(const Yaml& c) {
    CPU_SCOPE_TIMER("Foliage::load");
    // TODO: remove hardcoded trees
    std::vector<Yaml> treesToLoad;
    treesToLoad.emplace_back(resPath + "foliage/trees/Z_Trees.yml");

    ResourceLoader loader;
    // ? loader.meshPath = resPath + "foliage/trees/";

    // auto &&atlasData = prepareAtlas();
    textureAtlasID = assets::getAlbedoArray("Foliage").id;
    auto findTextureID = assets::layerSearch(assets::getAlbedoArray("Foliage"));
    ModelLoader<VertexSimple> modelLoader;

    modelLoader.open(resPath + "foliage/trees/"s + "Z_Trees.dae",
                     assets::layerSearch(assets::getAlbedoArray("Foliage")));
    for(const auto& tree : treesToLoad[0]) {
        i32 lodCount(-1);
        trees.push_back({tree.key()});
        for(const auto& lod : tree) {
            TreeMesh treeMesh;
            lodCount++;

            treeMesh.crown = modelLoader.beginMesh();
            for(const auto& obj : lod["Crown"]) {
                auto meshInternal = modelLoader.loadOnly(obj["Mesh"].string());
                modelLoader.setTextureLayer(meshInternal, findTextureID(obj["Texture"].string()));
            }
            modelLoader.endMesh(treeMesh.crown);

            treeMesh.trunk = modelLoader.beginMesh();
            for(const auto& obj : lod["Trunk"]) {
                auto meshInternal = modelLoader.loadOnly(obj["Mesh"].string());
                modelLoader.setTextureLayer(meshInternal, findTextureID(obj["Texture"].string()));
            }
            modelLoader.endMesh(treeMesh.trunk);
            trees.back().mesh[lodCount] = treeMesh;
        }
        for(i32 i = lodCount; i < 5; i++) { trees.back().mesh[i] = trees.back().mesh[lodCount]; }
    }

    meshVAO = modelLoader.build();

    // densitySampler = make_shared<Sampler2D>(resPath + "textures/TreeDensity.png");

    treeCapsuleShape = new btCapsuleShapeZ(0.1f, 10.f);
    treeCapsuleCompoundShape = new btCompoundShape();
    btTransform tr;
    tr.setIdentity();
    // tr.setOrigin(btVector3(0,0,0));
    tr.setOrigin(btVector3(0, 0, treeCOMOffset));
    // tr.setOrigin(btVector3(0,0,5+0.1f));
    treeCapsuleCompoundShape->addChildShape(tr, treeCapsuleShape);
    treeCapsuleCompoundShape->recalculateLocalAabb();

    RenderData::removeTreeFromPhysics = [this](btRigidBody*& body, btTypedConstraint* constraint) {
        physics.m_dynamicsWorld->removeConstraint(constraint);
        physics.removeBody(body);
        body = nullptr;
    };
    return true;
}

void Foliage::plantTrees(FoliageData& data, QTNode& node, QuadTree& QT) {
    std::default_random_engine rng(node.id + /*seed*/ 0);
    std::uniform_real_distribution<float> d(-node.dimensions.x * 0.49f, node.dimensions.x * 0.49f);
    std::uniform_real_distribution<float> rotation(0, 6.28);
    std::uniform_int_distribution<u32> random(0, 1024);

    std::vector<glm::vec4> plantedTrees;
    auto pushTree = [&](glm::vec4& p) {
        glm::vec4 cp(p);
        for(u32 i = 0; i < 5; i++) {
            glm::vec2 force(0);
            for(auto& f : plantedTrees) { force += f.w / glm::distance(cp.xy(), f.xy()); }
            glm::vec2 resistForce = glm::normalize(cp.xy() - p.xy()) * p.w / glm::distance(cp.xy(), p.xy());
            cp.x += force.x - resistForce.x;
            cp.y += force.y - resistForce.y;
        }
        plantedTrees.push_back(cp);
    };

    auto sample = [&, this](glm::vec4& position) -> bool {
        auto result = QT.sampleHeight(position, &node);
        position.z = result.position.z;
        position.w = 0.f;
        // if(result.normal.z < 0.93) position.z = -100000;
        return result.normal.z > 0.9;
    };

    u32 treesCount = densityMap.sample(node.center.xy()) * 5;
    // u32 treesCount = random(rng)%MAX_TREES_PER_NODE+1;
    for(u32 i = 0; i < treesCount; i++) {
        glm::vec4 position(d(rng), d(rng), 0, 1);
        // pushTree(position);
        position += node.center;
        sample(position);

        glm::mat4 transform = glm::translate(position.xyz()) * glm::rotate(rotation(rng), Z3);

        data.treeID.push_back(treeBatchData.push(trees[random(rng) % trees.size()], &node, {transform}));
    }

    u32 shrubsCount = std::max(4u, random(rng) % MAX_SHRUB_PER_NODE) * 0;
    for(u32 i = 0; i < shrubsCount; i++) {
        glm::vec4 position(node.center.x + d(rng), node.center.y + d(rng), 0, 1);
        sample(position);

        glm::mat4 transform = glm::translate(position.xyz()) * glm::rotate(rotation(rng), Z3);

        data.shrubID.push_back(shrubBatchData.push(shrubs[1], &node, {transform}));
    }
}

void RenderData::cleanup(std::vector<u32> FoliageData::*member) {
    auto trim = [this]() {
        for(i32 i = count - 1; i >= 0; --i) {
            if(info[i].owner == nullptr) {
                count--;
                if(info[i].rgBody)
                    removeTreeFromPhysics(info[i].rgBody, info[i].rgJoint);
            }
            else
                return;
        }
    };
    auto overwrite = [this, &member](i32 from, i32 to) {
        std::swap(crown.counts[to], crown.counts[from]);
        std::swap(crown.start[to], crown.start[from]);
        std::swap(trunk.counts[to], trunk.counts[from]);
        std::swap(trunk.start[to], trunk.start[from]);
        std::swap(uniforms[to], uniforms[from]);
        // find and change index in payload
        for(auto& it : *(info[from].owner->payload.foliageData).*member) {
            if(it == from) {
                it = to;
                break;
            }
        }
        std::swap(info[to].owner, info[from].owner);
    };

    trim();
    for(i32 i = 0; i < count; i++) {
        if(info[i].owner == nullptr) {
            overwrite(count - 1, i);
            count--;
        }
        trim();
    }
}

// trawę sadzimy na lod 1
void Foliage::update(glm::vec4 reference) {
    CPU_SCOPE_TIMER("Foliage::update");
    densityMap.reloadIfNeeded();
    densityMap.drawDebug();

    // no fisrt collect visible lod 1 chunks
    // first we have to collect indices from removed chunks
    // indices contain ids of trees in uniformData
    { // remove
        QtChunks removedChunks;
        for(auto& it : QT.lodLevels[0].removedNodes) {
            if(not it->parent->areAnyChildVisible())
                removedChunks.insert(it->parent);
        }
        for(auto& it : QT.lodLevels[1].removedNodes) { removedChunks.insert(it); }
        for(auto& it : QT.lodLevels[2].removedNodes) {
            // removedChunks.insert(it->children[0]);
            // removedChunks.insert(it->children[1]);
            // removedChunks.insert(it->children[2]);
            // removedChunks.insert(it->children[3]);
        }
        for(auto& it : removedChunks) {
            if(not it->payload.foliageData)
                continue;
            for(auto& t : it->payload.foliageData->treeID) treeBatchData.info[t].owner = nullptr;
            for(auto& t : it->payload.foliageData->shrubID) shrubBatchData.info[t].owner = nullptr;
            it->payload.foliageData.reset();
        }
        treeBatchData.cleanup(&FoliageData::treeID);
        shrubBatchData.cleanup(&FoliageData::shrubID);
    }
    QtChunks addedChunks;
    { // collect added
        for(auto& it : QT.lodLevels[0].addedNodes) {
            // if(not it->parent->payload and not it->parent->payload.foliageData)
            addedChunks.insert(it->parent);
        }
        for(auto& it : QT.lodLevels[1].addedNodes) { addedChunks.insert(it); }
        for(auto& it : QT.lodLevels[2].addedNodes) {
            // addedChunks.insert(it->children[0]);
            // addedChunks.insert(it->children[1]);
            // addedChunks.insert(it->children[2]);
            // addedChunks.insert(it->children[3]);
        }
    }

    // set owner of removed render data to 0

    for(auto& added : addedChunks) {
        if(not added->payload.foliageData)
            added->payload.foliageData = std::make_unique<FoliageData>();
        else
            continue;
        plantTrees(*added->payload.foliageData, *added, QT);
    }

    treeBatchData.cleanup(&FoliageData::treeID);
    shrubBatchData.cleanup(&FoliageData::shrubID);

    treeBatchData.updatePhysics(reference, physics);
}
// TODO: errr
void RenderData::updatePhysics(glm::vec4 reference, PhysicalWorld& p) {
    auto addTree = [&](glm::mat4 o, FoliagePhysics& f) {
        btTransform tr(btMatrix3x3(o[0][0], o[1][0], o[2][0], o[0][1], o[1][1], o[2][1], o[0][2], o[1][2], o[2][2]),
                       btVector3(o[3][0], o[3][1], o[3][2])
                           + btVector3(o[2][0], o[2][1], o[2][2]) * (5 - treeCOMOffset + 0.2));
        f.rgBody = p.createRigidBody(200, tr, treeCapsuleCompoundShape, 1);
        // f.rgBody = p.createRigidBody(90, tr, treeCapsuleShape, 3);
        f.rgBody->setDamping(0.9, 0.4);
        // f.rgBody->setGravity(btVector3(0,0,-10));

        auto trA = tr;
        auto trB = tr;

        trB = btIdentity;
        // trB.getOrigin() += btVector3(0,0,-2);
        // trA.getOrigin() += btVector3(0,0,-(5-treeCOMOffset+0.1));
        // f.rgJoint = new btGeneric6DofSpring2Constraint(*rgTerrain, *f.rgBody, trA, trB);
        // f.rgJoint = new btGeneric6DofSpring2Constraint(*f.rgBody, tr);
        f.rgJoint->setLinearLowerLimit(btVector3(0, 0, 0));
        f.rgJoint->setLinearUpperLimit(btVector3(0, 0, 1.5));
        f.rgJoint->setBreakingImpulseThreshold(10000);
        // f.rgJoint->setAngularLowerLimit(btVector3(-0.50, -0.50, -35*toRad));
        // f.rgJoint->setAngularUpperLimit(btVector3(0.50, 0.50, 35*toRad));
        // f.rgJoint->setAngularLowerLimit(btVector3(0,0,0));
        // f.rgJoint->setAngularUpperLimit(btVector3(0,0,0));
        // f.rgJoint->enableMotor(3, true);
        // f.rgJoint->enableMotor(4, true);
        f.rgJoint->enableMotor(5, true);
        // f.rgJoint->setTargetVelocity(3, 0);
        // f.rgJoint->setTargetVelocity(4, 0);
        f.rgJoint->setTargetVelocity(5, 0);
        // f.rgJoint->setMaxMotorForce(3, 1000);
        // f.rgJoint->setMaxMotorForce(4, 1000);
        f.rgJoint->setMaxMotorForce(5, 1000);
        // f.rgJoint->setEquilibriumPoint();
        // f.rgJoint->setBounce(3, 100);
        // f.rgJoint->setBounce(4, 100);
        // f.rgJoint->setBounce(5, 100);
        // f.rgJoint->setStiffness(2, 50);

        f.rgJoint->enableSpring(3, true);
        f.rgJoint->enableSpring(4, true);
        f.rgJoint->enableSpring(5, true);
        f.rgJoint->setStiffness(3, 8000);
        f.rgJoint->setStiffness(4, 8000);
        f.rgJoint->setStiffness(5, 8000);
        f.rgJoint->setDamping(3, 0.1);
        f.rgJoint->setDamping(4, 0.1);
        f.rgJoint->setDamping(5, 0.1);
        f.rgJoint->setEquilibriumPoint();
        //
        // f.rgJoint->setServo(3, true);
        // f.rgJoint->setServo(4, true);
        // f.rgJoint->setServo(5, true);
        //
        // f.rgJoint->setServoTarget(3, 0);
        // f.rgJoint->setServoTarget(4, 0);
        // f.rgJoint->setServoTarget(5, 0);

        p.m_dynamicsWorld->addConstraint(f.rgJoint);
    };
    auto getLod = [&](float distance) -> i32 {
        return 0;
        for(i32 i = 0; i < 5; i++) {
            if(distance < lodDistances[i])
                return i;
        }
        return 5;
    };
    auto updateTreePhysics = [&](FoliagePhysics& f, FoliageUniform& u) {
        auto tr = f.rgBody->getCenterOfMassTransform();
        // u.transform = convert(tr);
        u.transform = convert(tr) * glm::translate(glm::vec3(0, 0, -(5 - treeCOMOffset + 0.2)));

        float treeDeflection = acos(tr.getBasis().getColumn(2).dot(btVector3(0, 0, 1)));
        if(treeDeflection > 30 * toRad and not f.cracked) {
            f.cracked = true;
            f.rgJoint->enableSpring(3, false);
            f.rgJoint->enableSpring(4, false);
            f.rgJoint->enableSpring(5, false);
            // f.rgJoint->setAngularLowerLimit(btVector3(0.5, 0.5, -0.1));
            // f.rgJoint->setAngularUpperLimit(btVector3(-0.5, -0.5, 0.1));
            // p.dynamicsWorld->removeConstraint(f.rgJoint);
            // f.rgJoint = nullptr;

            console.log("Tree cracked!", treeDeflection);
        }
    };
    u32 physicalTrees(0);
    ShadowRenderers.clear();

    const float distanceTreshold = -50.f;
    const float ShadowRendererTreshold = 190.f;
    glm::vec4 shadowCenter(0);
    std::vector<glm::vec4> points;

    oscillation = glm::vec4(glm::perlin(glm::vec4(/*time*/ 15 / 1000.f * 1.2 / 5))) * 0.1f;
    // oscillation = glm::vec4(sin(Global::time/100.f*1.2/5), sin(Global::time/100.f*1.2/5), 0, 0)*0.1f;
    for(u32 i = 0; i < count; i++) {
        uniforms[i].motion = oscillation;

        glm::vec4 position = glm::column(uniforms[i].transform, 3);
        float distance = glm::distance(reference, position);
        if(distance < ShadowRendererTreshold) {
            ShadowRenderers.push_back(i);
            points.push_back(position);
            shadowCenter += position;
        }
        if(distance < distanceTreshold) {
            if(not info[i].rgBody) {
                physicalTrees++;
                addTree(uniforms[i].transform, info[i]);
            }
            else {
                updateTreePhysics(info[i], uniforms[i]);
            }
        }
        else if(info[i].rgBody) {
            removeTreeFromPhysics(info[i].rgBody, info[i].rgJoint);
        }

        auto lod = getLod(distance);
        if(lod >= 5) {
            crown.counts[i] = 0; // then mesh in not drawn
            trunk.counts[i] = 0; // then mesh in not drawn
        }
        else {
            // crown.counts[i] = info[i].model->mesh[lod].crown.count;
            // crown.start[i] = info[i].model->mesh[lod].crown.begin;
            // trunk.counts[i] = info[i].model->mesh[lod].trunk.count;
            // trunk.start[i] = info[i].model->mesh[lod].trunk.begin;
        }
    }
}

// ------ FOLAIGE DENSITY ------
const float maxTrees = 20;
FoliageDensityMap::FoliageDensityMap(glm::vec2 worldSize, glm::ivec2 nodes) {
    sampler.transformInput = [=](glm::vec2 p) -> glm::vec2 {
        // console.log("input:", p, "mid", p/worldSize, "output", glm::repeat(p/worldSize));
        return glm::repeat(p / worldSize + glm::vec2(0.5f));
    };
    sampler.transformOutput = [](u8 value) { return value / 11; };
    sampler.size = nodes / 2;
    console.log("FoliageDensityMap size:", sampler.size);

    std::vector<u8> density(sampler.size.x * sampler.size.y, 0);
    sampler.data = std::move(density);
    auto needReload = [this]() { isReloadNeeded = true; };
    generate();
}
void FoliageDensityMap::generate() {
    for(float x = 0; x < sampler.size.x; x += 1)
        for(float y = 0; y < sampler.size.y; y += 1) {
            float a = scaled_octave_noise_2d(octaves, persistence, scale, 0, 255, x * seed + seedOffset,
                                             y * seed + seedOffset);
            auto v = (255 - a) - threshold;
            // auto v = (1-sin(a/255.f))*255-threshold;
            sampler.data[x + y * sampler.size.x] = v > 0 ? v : 0;
        }

    // sampler.data[0] = 255;
    // sampler.data[7] = 255;
    // sampler.data[63] = 255;
    console.log("*****", sampler.data.size());
    upload();
}
void FoliageDensityMap::upload() {
    if(textureId == 0) {
        gl::GenTextures(1, &textureId);
        gl::BindTexture(gl::TEXTURE_2D, textureId);

        gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::REPEAT);
        gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::REPEAT);
        gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, gl::NEAREST);
        gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, gl::NEAREST);

        gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RED, sampler.size.x, sampler.size.y, 0, gl::RED, gl::UNSIGNED_BYTE,
                       sampler.data.data());

        gl::BindTexture(gl::TEXTURE_2D, 0);
    }
    else {
        gl::BindTexture(gl::TEXTURE_2D, textureId);
        gl::TexImage2D(gl::TEXTURE_2D, 0, gl::RED, sampler.size.x, sampler.size.y, 0, gl::RED, gl::UNSIGNED_BYTE,
                       sampler.data.data());
        gl::BindTexture(gl::TEXTURE_2D, 0);
    }
}
void FoliageDensityMap::reloadIfNeeded() {
    if(not isReloadNeeded)
        return;
    generate();
    isReloadNeeded = false;
}
void FoliageDensityMap::drawDebug() {}
u8 FoliageDensityMap::sample(glm::vec2 p) {
    return sampler.nearest(p);
}
