#include "core.hpp"
#include "SceneGraph.hpp"
#include "ModelLoader.hpp"
#include "Assets.hpp"
#include "PhysicalWorld.hpp"
#include "Yaml.hpp"
#include "Utils.hpp"
#include "PerfTimers.hpp"
#include "BroadphaseFrustumCulling.hpp"


const float manhattanLodDistances[4] = {15,60,100,900};

SceneGraph::SceneGraph(PhysicalWorld &physics) : cells(pow(4, NoOfLevels)), physics(physics){
    defaultSphereCollider = new btSphereShape(3);
}

void SceneGraph::diffBetweenFrames(){
    std::sort(addedCells.begin(), addedCells.end());
    std::vector<i32> justAddedCells;
    justAddedCells.swap(addedCells);

    u32 oldS = visibleCells.size();
    u32 curS = justAddedCells.size();
    u32 oldI = 0;
    u32 curI = 0;
    for(; oldI<oldS && curI<curS;){
        if(visibleCells[oldI] == justAddedCells[curI]){
            oldI++;
            curI++;
        }
        else if(visibleCells[oldI] < justAddedCells[curI]){
            removedCells.push_back(visibleCells[oldI]);
            oldI++;
        }
        else {
            addedCells.push_back(justAddedCells[curI]);
            curI++;
        }
    }
    for(; oldI<oldS; oldI++){
        removedCells.push_back(visibleCells[oldI]);
    }
    for(; curI<curS; curI++){
        addedCells.push_back(justAddedCells[curI]);
    }

    visibleCells.swap(justAddedCells);
}

void SceneGraph::setLodForVisible(glm::vec4 eye){
    for(auto &i : visibleCells){
        auto &cell = cells.at(i);

        auto vec = glm::abs(cell.position - glm::ceil(eye/cell.size.x*10.f)*cell.size.x/10.f);
        auto distance = std::max(vec.x, vec.y);

        if(distance < manhattanLodDistances[0]) cell.level = 0;
        else if(distance < manhattanLodDistances[1]) cell.level = 1;
        else if(distance < manhattanLodDistances[2]) cell.level = 2;
        else if(distance < manhattanLodDistances[3]) cell.level = 3;
        else cell.level = -1;
    }
}


void SceneGraph::cullWithPhysicsEngine(const Frustum &frustum){
    CPU_SCOPE_TIMER("cullWithPhysicsEngine");
    btDbvtBroadphase *dbvtBroadphase = dynamic_cast<btDbvtBroadphase*>(physics.broadphase);
    if(not dbvtBroadphase){
        error("wrong broadphase type or uninitialized");
        return;
    }

    DbvtBroadphaseFrustumCulling culling;

    btVector3 normals[] = {
        -convert(frustum.planes.m.rightPlane.xyz()),
        -convert(frustum.planes.m.leftPlane.xyz()),
        -convert(frustum.planes.m.topPlane.xyz()),
        -convert(frustum.planes.m.bottomPlane.xyz()),
        -convert(frustum.planes.m.farPlane.xyz())
    };
    btScalar offsets[] = {
        -btScalar(frustum.planes.m.rightPlane.w),
        -btScalar(frustum.planes.m.leftPlane.w),
        -btScalar(frustum.planes.m.topPlane.w),
        -btScalar(frustum.planes.m.bottomPlane.w),
        -btScalar(frustum.planes.m.farPlane.w)
    };
    bool cullFarPlane = false;
    btDbvt::collideKDOP(dbvtBroadphase->m_sets[1].m_root, normals, offsets, cullFarPlane ? 5 : 4, culling); // with static
    btDbvt::collideKDOP(dbvtBroadphase->m_sets[0].m_root, normals, offsets, cullFarPlane ? 5 : 4, culling); // with dynamic
    visibleObjects.clear();
    for(auto &it : culling.objectsInsideFrustum){
        auto obj = objects[it];
        visibleObjects[obj.type].push_back(obj);
    }
}

void SceneGraph::cullCells(const Frustum &frustum){
    cullWithPhysicsEngine(frustum);
    diffBetweenFrames();
    setLodForVisible(frustum.eye);
}

void SceneGraph::loadCollider(ModelLoader &loader, const std::string &name){
    auto data = loader.loadStatic3DMesh(name);

    colliderMesh = new btTriangleMesh();

    auto *points = reinterpret_cast<const btVector3*>(data.first.data());
    const auto &indices = data.second;
    for(auto i=0; i<indices.size(); i+=3){
        colliderMesh->addTriangle(points[indices[i]],points[indices[i+1]],points[indices[i+2]]);
    }
    colliderShape = new btBvhTriangleMeshShape(colliderMesh, true);

    btTransform tr;
    tr.setIdentity();
    collider = physics.createRigidBody(0, tr, colliderShape);
    collider->setUserPointer(this);
    collider->setUserIndex(Object::nextID());
}
btRigidBody* SceneGraph::createSimpleCollider(glm::vec4 pos, glm::vec3 dim){
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(convert(pos));
    auto body = physics.createRigidBodyWithMasks(0, tr, new btBoxShape(btVector3(dim.x*0.5f, dim.y*0.5f, dim.z*0.8f)), nullptr, 2, 0);

    return body;
}

/*
    Wyciąga i przesuwa submeshe w odpowiednie miejsca, dodaje do vao
    */
void SceneGraph::loadMap(const std::string &mapConfigDir){
    Yaml map(mapConfigDir + "/map.yml");

    glm::vec4 min = map["Terrain"]["Min"].vec4();
    glm::vec4 max = map["Terrain"]["Max"].vec4();
    center = (min + max)/2.f;

    auto dim = std::max(max.x-min.x, max.y-min.y) + 4;

    nodes = glm::vec2(32);
    size = glm::vec4(dim, dim, max.z - min.z, 0);

    log("size:", size, "center", center);

    ModelLoader loader;
    loader.loadUV = 0;
    loader.debug = false;
    loader.open(mapConfigDir + "/map.dae");

    cells.resize(map["Terrain"]["Chunks"].size());
    int i=0;
    for(auto &it : map["Terrain"]["Chunks"]){
        if(it["isCollider"].boolean()){
            // auto mesh3D = loader.getBullet3DMesh(it["Mesh"].string());
            // but to bullet
        }

        auto intMesh = loader.getInternalMesh(it["Mesh"].string());

        auto position = it["Position"].vec4();
        auto dimension = it["Dimension"].vec30();
        auto count = intMesh.vertex.size();
        for(auto i=0; i<count; i+=4){
            intMesh.vertex[i+0] += position[0];
            intMesh.vertex[i+1] += position[1];
            intMesh.vertex[i+2] += position[2];
            intMesh.vertex[i+3] += position[3]*0;
        }

        cells[i].position = position;
        cells[i].size = dimension;
        cells[i].level = 0;
        cells[i].hasTerrain = true;
        cells[i].terrainMesh = loader.insert(intMesh).toMesh();
        cells[i].cellBoxCollider = createSimpleCollider(position, dimension.xyz());

        Object object {Type::TerrainChunk, Object::nextID(), &cells[i], i};

        cells[i].cellBoxCollider->setUserIndex(object.ID);
        objects[object.ID] = object;
        i++;
    }

    vao = loader.build();
    assets::addVao(vao, "Terrain");

    loadCollider(loader, map["Terrain"]["Collider"].string());
}

SampleResult SceneGraph::sample(glm::vec4 position){
    btVector3 from(double(position.x), double(position.y), double(position.z + 500.f));
    btVector3 to(double(position.x), double(position.y), double(position.z - 500.f));

    btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
    physics.dynamicsWorld->rayTest(from, to, allResults);

    if(allResults.hasHit()){
        u32 minI=0;
        for(u32 i=0; i<allResults.m_hitPointWorld.size(); i++ ){
            if(allResults.m_collisionObjects[i]->getUserPointer() != this) continue;
            if(allResults.m_hitFractions[i] < allResults.m_hitFractions[minI]) {
                minI = i;
            }
        }
        return {convert(allResults.m_hitPointWorld[minI],1), convert(allResults.m_hitNormalWorld[minI], 0), true};
    }
    else {
        return {{0,0,-500,0}};
    }
}
