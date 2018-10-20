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

void Environment::loadObject(const Yaml &thing, ModelLoader<VertexWithMaterialData>& modelLoader){
    EnviroEntity e {};
    e.name = thing["Name"].string();

    auto x = thing["Position"]["X"].vec4();
    auto y = thing["Position"]["Y"].vec4();
    auto z = thing["Position"]["Z"].vec4();
    auto w = thing["Position"]["W"].vec31();

    e.physics.position = w;
    e.physics.transform = glm::mat4(x,y,z,w);

    loadVisualPart(modelLoader, e, thing);
    loadPhysicalPart(modelLoader, e, thing);

    // if(thing["isPhysical"].boolean() and thing["Colliders"].string() != "none"){
    //     auto colliders = modelLoader.loadConvexMeshes({thing["Colliders"].strings()});
        // e.physics.rgBody = physics->createRigidBody(0, convert(obj["Quaternion"].quat(), e.physics.position), createCompoundMesh(colliders, nullptr));
    // }
    e.id = m_entities.size();
    m_entities.push_back(e);
    // SceneObject object{Type::Enviro, SceneObject::nextID(), nullptr, e.id};
    // if(e.physics.rgBody) e.physics.rgBody->setUserIndex(object.ID);

    graph.insertObject(m_entities.back().getProvider(), e.physics.position);
}


void Environment::loadVisualPart(ModelLoader<VertexWithMaterialData>& modelLoader, EnviroEntity &e, const Yaml &yaml){
    e.graphic.mesh = modelLoader.load(yaml["Models"].strings());
}

void Environment::loadPhysicalPart(ModelLoader<VertexWithMaterialData>& modelLoader, EnviroEntity &e, const Yaml &yaml){
    if(not yaml["isPhysical"].boolean()) return;
    for(auto &collider : yaml["Colliders"]){
        // load colliders
    }

    btCollisionShape *shape = nullptr;

    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(yaml["Position"].btVec());

    e.physics.rgBody = physics.createRigidBody(0, tr, shape);
}

void Environment::loadLightSource(const Yaml &thing){
    auto &l = m_lights.emplace_back(thing);
    // auto collider = l->getCollider();

    graph.insertObject(l.getProvider(), l->m_position);
}

void Environment::update(float dt){
    for(auto & it : m_entities) it->update(dt);
    for(auto & it : m_lights) it->update(dt);
}
