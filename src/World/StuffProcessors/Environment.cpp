#include "core.hpp"
#include "Environment.hpp"
#include "GPUResources.hpp"
#include "LightSource.hpp"
#include "Logging.hpp"
#include "ModelLoader.hpp"
#include "PerfTimers.hpp"
#include "PhysicalWorld.hpp"
#include "RenderDataCollector.hpp"
#include "ResourceLoader.hpp"
#include "Utils.hpp"
#include "Yaml.hpp"

void EnviroEntity::update(float dt){}
void EnviroEntity::actionWhenVisible(){
    if(lastFrame==frame()) return; // * to be sure that object will be inserted once per frame :)
    lastFrame = frame();

    RenderDataCollector::enviro.push(graphic.mesh.count, graphic.mesh.offset(), physics.transform);
}


void Environment::load(const std::string &sceneName){
    CPU_SCOPE_TIMER("Environment::load");

    std::string path = resPath + "scenes/" + sceneName + "/" + sceneName;

    ModelLoader<VertexWithMaterialData> modelLoader;
    // modelLoader.loadTangents = false; // ! TODO: co jest nie tak z tangentami?
    // modelLoader.m_uvSize = 3;
    modelLoader.open(path + ".dae", std::move(assets::layerSearch(assets::getAlbedoArray("Materials"))));
    if(not modelLoader.good){
        error("Unable to load environment");
        return;
    }

    Yaml yaml(path + ".yml");
    modelLoader.materials = yaml["Materials"];
    for(auto& it : yaml["Objects"]) loadObject(it, modelLoader);
    if(yaml.has("LightSources")) for(auto& it : yaml["LightSources"]) loadLightSource(it);
    vao = modelLoader.build();
    RenderDataCollector::enviro.vao = vao;

    const Yaml &lamps = yaml["LightSources"];
}

void Environment::loadObject(const Yaml &yaml, ModelLoader<VertexWithMaterialData>& modelLoader){
    auto& entity = m_entities.emplace_back(yaml["Name"].string());

    auto x = yaml["Position"]["X"].vec4();
    auto y = yaml["Position"]["Y"].vec4();
    auto z = yaml["Position"]["Z"].vec4();
    auto w = yaml["Position"]["W"].vec31();

    entity.physics.position = w;
    entity.physics.dimensions = yaml["Dimensions"].vec30();
    entity.physics.transform = glm::mat4(x,y,z,w);
    entity.id = m_entities.size();

    loadVisualPart(modelLoader, entity, yaml);

    //* most of objects needs to be physical, for collisions, pathfinding and culling
    if(not loadPhysicalPart(modelLoader, entity, yaml))
        createSimpleCollider(entity);


    // * save object id in rigid body
    if(entity.physics.rgBody) entity.physics.rgBody->setUserIndex(entity.indexForBullet());

    graph.insertObject(entity.getHandle(), entity.physics.position);
}

void Environment::loadVisualPart(ModelLoader<VertexWithMaterialData>& modelLoader, EnviroEntity &e, const Yaml &yaml){
    e.graphic.mesh = modelLoader.load(yaml["Models"].strings());
}

void Environment::createSimpleCollider(EnviroEntity &entity){
    entity.physics.shape = new btBoxShape(convert(entity.physics.dimensions));
    btTransform tr = convert(entity.physics.transform);
    entity.physics.rgBody = physics.createRigidBody(0, tr, entity.physics.shape);
}

bool Environment::loadPhysicalPart(ModelLoader<VertexWithMaterialData>& modelLoader, EnviroEntity &entity, const Yaml &yaml){
    if(not yaml["isPhysical"].boolean()) return false;
    if(yaml["isPhysical"].boolean() and yaml["Colliders"].string() != "none"){
        auto colliders = modelLoader.loadConvexMeshes({yaml["Colliders"].strings()});
        entity.physics.shape = createCompoundShape(colliders, nullptr);

        entity.physics.rgBody = physics.createRigidBody(0, convert(entity.physics.transform), entity.physics.shape);
    }


    btTransform tr = convert(entity.physics.transform);
    entity.physics.rgBody = physics.createRigidBody(0, tr, entity.physics.shape);

    return true;
}

void Environment::loadLightSource(const Yaml &thing){
    auto &l = m_lights.emplace_back(thing);
    // auto collider = l->getCollider();

    graph.insertObject(l.getHandle(), l.m_position);
}

void Environment::update(float dt){
    for(auto & it : m_entities) it.update(dt);
    for(auto & it : m_lights) it.update(dt);
}
