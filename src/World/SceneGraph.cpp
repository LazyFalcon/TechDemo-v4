#include "core.hpp"
#include "SceneGraph.hpp"
#include "Assets.hpp"
#include "BroadphaseFrustumCulling.hpp"
#include "ModelLoader.hpp"
#include "PerfTimers.hpp"
#include "PhysicalWorld.hpp"
#include "Utils.hpp"
#include "Yaml.hpp"

void Cell::addToSceneVisibleList(visuals::PreparedScene& preparedScene) {
    // console.clog("cell is visible");
    for(auto& it : objects) { it->addItselfToSceneVisibleList(preparedScene); }
}

void Cell::addToShadowCastingList(model::Collection& collection) {
    // console.clog("cell is visible");
    // tu powinno się trochę więcej dziać, np. dodać całą skeszowaną paczkę, zamiast pojedynczo
    for(auto& it : objects) { it->addItselfToShadowCastingList(collection); }
}

const float manhattanLodDistances[4] = {15, 60, 100, 900};

SceneGraph::SceneGraph(PhysicalWorld& physics) : cells(pow(4, NoOfLevels)), physics(physics) {
    defaultSphereCollider = new btSphereShape(3);
}

void SceneGraph::initAndLoadMap(const Yaml& yaml) {
    min = yaml["Bounds"]["Min"].vec4();
    max = yaml["Bounds"]["Max"].vec4();
    size = max - min;
    center = (min + max) / 2.f;
    initCellsToDefaults();

    if(yaml["hasTerrainHeightmap"].boolean())
        loadMap("");
}

void SceneGraph::initCellsToDefaults() {
    // * adjust cell sie to fit in world
    auto idealCellSize = glm::vec2(32);
    cellsInTheScene = glm::clamp(glm::floor(size.xy() / idealCellSize), glm::vec2(1), glm::vec2(256));
    cellSize = size.xy() / cellsInTheScene;

    cells.resize(cellsInTheScene.x * cellsInTheScene.y);

    console.log("dim:", size.xy());
    console.log("cellSize:", cellSize);
    console.log("cellsInTheScene:", cellsInTheScene);
    int i = 0;
    // todo: do we need to create collider for each cell? wouldn't be better to create them when something has to be inserted?
    // todo: do it when game will be running again
    for(int x = 0; x < cellsInTheScene.x; x++)
        for(int y = 0; y < cellsInTheScene.y; y++) {
            glm::vec4 position = min + glm::vec4(cellSize.x * (0.5f + x), cellSize.y * (0.5f + y), center.z, 0);
            cells[i].position = position;
            cells[i].size = glm::vec4(cellSize, 300.f, 0.f);
            cells[i].level = 0;
            cells[i].id = i;
            cells[i].hasTerrain = false;
            cells[i].cellBoxCollider = createSimpleCollider(cells[i].position, cells[i].size.xyz());

            // SceneObject object {Type::TerrainChunk, SceneObject::nextID(), &cells[i], i};

            cells[i].setRef(cells[i].cellBoxCollider);
            i++;
        }
}

btRigidBody* SceneGraph::createSimpleCollider(glm::vec4 pos, glm::vec3 dim) {
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(convert(pos));
    int group = COL_FOR_CULLING;
    int collideWith = COL_NOTHING;
    auto body = physics.createRigidBodyWithMasks(
        0, tr, new btBoxShape(btVector3(dim.x * 0.5f, dim.y * 0.5f, dim.z * 0.8f)), group, collideWith);

    return body;
}

void SceneGraph::insertObject(ObjectHandle object, const glm::vec4& position) {
    if(object->getCollider())
        return; // * no need to track it now, bullet will do it better
    console.log("Inserting object to cell");
    auto cell = findCellUnderPosition(position);
    if(not cell) {
        console.error("cell under", position, "doesn't exists");
        return;
    }
    cell->objects.push_back(object);
}

Cell* SceneGraph::findCellUnderPosition(const glm::vec4& pos) {
    if(pos.x < min.x or pos.x > max.x or pos.y < min.y or pos.y > max.y)
        return nullptr;

    glm::vec2 p = pos.xy() - min.xy();
    p /= cellSize;

    return &(cells[int(p.x) + int(p.y) * cellsInTheScene.x]);
}

void SceneGraph::findObjectsInsideFrustum(const camera::Frustum& frustum) {
    CPU_SCOPE_TIMER("findObjectsInsideFrustum");
    btDbvtBroadphase* dbvtBroadphase = physics.m_broadphase;
    DbvtBroadphaseFrustumCulling culling;

    btVector3 normals[] = {-convert(frustum.planes.m.rightPlane.xyz()), -convert(frustum.planes.m.leftPlane.xyz()),
                           -convert(frustum.planes.m.topPlane.xyz()), -convert(frustum.planes.m.bottomPlane.xyz()),
                           -convert(frustum.planes.m.farPlane.xyz())};
    btScalar offsets[] = {-btScalar(frustum.planes.m.rightPlane.w), -btScalar(frustum.planes.m.leftPlane.w),
                          -btScalar(frustum.planes.m.topPlane.w), -btScalar(frustum.planes.m.bottomPlane.w),
                          -btScalar(frustum.planes.m.farPlane.w)};

    // std::array < std::vector<uint>, ile tych typów jest ? > listsOfObjects;
    bool cullFarPlane = false;
    btDbvt::collideKDOP(dbvtBroadphase->m_sets[1].m_root, normals, offsets, cullFarPlane ? 5 : 4,
                        culling); // with static
    btDbvt::collideKDOP(dbvtBroadphase->m_sets[0].m_root, normals, offsets, cullFarPlane ? 5 : 4,
                        culling); // with dynamic

    // addTolistOfVisibleObjects(listsOfObjects);
}

auto fitPlanesAroundFrustum(const std::array<glm::vec4, 5>& corners, glm::vec4 lightDirection) {
    auto transformToLightSpace =
        glm::lookAt(corners[0].xyz() - lightDirection.xyz(), corners[0].xyz(), glm::vec3(0, 0, 1));
    std::array<glm::vec4, 5> cornersInLightSpace;
    std::transform(corners.begin(), corners.end(), cornersInLightSpace.begin(),
                   [&transformToLightSpace](const glm::vec4& in) { return transformToLightSpace * in; });

    auto& eye = cornersInLightSpace[0];
    int left(0), right(0), top(0), bottom(0);
    for(int i = 0; i < 5; i++) {
        if(cornersInLightSpace[i].x < cornersInLightSpace[left].x)
            left = i;
        if(cornersInLightSpace[i].x > cornersInLightSpace[right].x)
            right = i;
        if(cornersInLightSpace[i].y < cornersInLightSpace[bottom].y)
            bottom = i;
        if(cornersInLightSpace[i].y > cornersInLightSpace[top].y)
            top = i;
    }

    std::array<glm::vec4, 5> planes;
    auto& c = cornersInLightSpace;
    glm::vec3 normalRight = glm::normalize(glm::cross(lightDirection.xyz(), glm::vec3(0, 0, 1)));
    glm::vec3 normalUp = glm::cross(normalRight, lightDirection.xyz());
    planes[0] = plane(normalRight, c[right]);
    planes[1] = plane(-normalRight, c[left]);
    planes[2] = plane(normalUp, c[top]);
    planes[3] = plane(-normalUp, c[bottom]);
    planes[3] = plane(glm::cross(glm::normalize(c[right].xyz() - c[left].xyz()), normalUp), (c[right] - c[left]) / 2.f);

    return planes;
}

void SceneGraph::findObjectsOutsideFrustumThatCastShadows(const camera::Frustum& frustum, glm::vec4 lightDirection) {
    CPU_SCOPE_TIMER("findObjectsOutsideFrustumThatCastShadows");
    btDbvtBroadphase* dbvtBroadphase = physics.m_broadphase;
    CullingForShadowmap culling;

    auto planes =
        fitPlanesAroundFrustum({frustum.eye, frustum.slices.slice[4].topRight, frustum.slices.slice[4].topLeft,
                                frustum.slices.slice[4].bottomRight, frustum.slices.slice[4].bottomLeft},
                               lightDirection);

    btVector3 normals[] = {-convert(planes[0].xyz()), -convert(planes[1].xyz()), -convert(planes[2].xyz()),
                           -convert(planes[3].xyz()), -convert(planes[4].xyz())};
    btScalar offsets[] = {-btScalar(planes[0].w), -btScalar(planes[1].w), -btScalar(planes[2].w),
                          -btScalar(planes[3].w), -btScalar(planes[4].w)};
    bool cullFarPlane = false;
    btDbvt::collideKDOP(dbvtBroadphase->m_sets[1].m_root, normals, offsets, 5,
                        culling); // with static
    // btDbvt::collideKDOP(physics.m_cullingClusters->m_sets[1].m_root, normals, offsets, 5,
    //                     culling); // with static
    btDbvt::collideKDOP(dbvtBroadphase->m_sets[0].m_root, normals, offsets, 5,
                        culling); // with dynamic
}

void SceneGraph::cullCells(const camera::Frustum& frustum) {
    findObjectsInsideFrustum(frustum);
    // diffBetweenFrames();
    // setLodForVisible(frustum.eye);
}

void SceneGraph::loadCollider(ModelLoader<VertexWithMaterialData>& loader, const std::string& name) {
    auto data = loader.loadStatic3DMesh(name);

    colliderMesh = new btTriangleMesh();

    auto* points = reinterpret_cast<const btVector3*>(data.first.data());
    const auto& indices = data.second;
    for(auto i = 0; i < indices.size(); i += 3) {
        colliderMesh->addTriangle(points[indices[i]], points[indices[i + 1]], points[indices[i + 2]]);
    }
    colliderShape = new btBvhTriangleMeshShape(colliderMesh, true);

    btTransform tr;
    tr.setIdentity();
    collider = physics.createRigidBody(0, tr, colliderShape);
    collider->setUserPointer(this);
    // collider->setUserIndex(SceneObject::nextID());
}

/*
* Check if config is image, that means terrain class will be(should be was) created
*   and hope that terrain could be splitted evenly -> TODO: this
* If not image then it's composed from 3D model; they are treated little differently than regular 3D models : dedicated lod, contaier and shaders
* -> meshes have LOD
* -> ? distributed on per cell(strange cuts) or many per cell and non unique?, second option looks better especially with uniqueness(uniqueness by frame number)
*/
void SceneGraph::loadMap(const std::string& mapConfigDir) {
    Yaml map(mapConfigDir + "/map.yml");

    auto dim = std::max(max.x - min.x, max.y - min.y);
    nodes = glm::vec2(32);
    size = glm::vec4(dim, dim, max.z - min.z, 0);

    console.log("size:", size, "center", center);

    ModelLoader<VertexWithMaterialData> loader;
    // loader.loadUV = 0;
    // loader.debug = false;
    loader.open(mapConfigDir + "/map.dae");

    // TODO: completly rework this according to decription above func definition
    int i = 0;
    for(auto& it : map["Terrain"]["Chunks"]) {
        if(it["isCollider"].boolean()) {
            // auto mesh3D = loader.getBullet3DMesh(it["Mesh"].string());
            // but to bullet
        }

        auto model = loader.load(it["Mesh"].string());

        auto position = it["Position"].vec4();
        auto dimension = it["Dimension"].vec30();
        // auto count = intMesh.vertex.size();
        loader.moveModel(model, position.xyz());

        // cells[i].position = position;
        // cells[i].size = dimension;
        // cells[i].level = 0;
        cells[i].hasTerrain = true;
        cells[i].terrainMesh = model;
        // cells[i].cellBoxCollider = createSimpleCollider(position, dimension.xyz());

        // SceneObject object {Type::TerrainChunk, SceneObject::nextID(), &cells[i], i};

        // cells[i].cellBoxCollider->setUserIndex(object.ID);
        // objects[object.ID] = object;
        i++;
    }

    vao = loader.build();
    assets::addVao(vao, "Terrain");

    loadCollider(loader, map["Terrain"]["Collider"].string());
}

SampleResult SceneGraph::sample(glm::vec4 position) {
    btVector3 from(double(position.x), double(position.y), double(position.z + 500.f));
    btVector3 to(double(position.x), double(position.y), double(position.z - 500.f));

    btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
    physics.m_dynamicsWorld->rayTest(from, to, allResults);

    if(allResults.hasHit()) {
        u32 minI = 0;
        for(u32 i = 0; i < allResults.m_hitPointWorld.size(); i++) {
            if(allResults.m_collisionObjects[i]->getUserPointer() != this)
                continue;
            if(allResults.m_hitFractions[i] < allResults.m_hitFractions[minI]) {
                minI = i;
            }
        }
        return {convert(allResults.m_hitPointWorld[minI], 1), convert(allResults.m_hitNormalWorld[minI], 0), true};
    }
    else {
        return {{0, 0, -500, 0}};
    }
}
