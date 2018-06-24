#include "core.hpp"
#include "Environment.hpp"
#include "Logging.hpp"
#include "ResourceLoader.hpp"
#include "LightSource.hpp"
#include "ModelLoader.hpp"
#include "GPUResources.hpp"
#include "PerfTimers.hpp"
#include "PhysicalWorld.hpp"
#include "Yaml.hpp"
#include "Utils.hpp"

void Environment::load(const std::string &sceneName){
    CPU_SCOPE_TIMER("Environment::load");

    std::string path = resPath + "scenes/" + sceneName + "/" + sceneName;

    ModelLoader modelLoader;
    modelLoader.loadTangents = false; // ! TODO: co jest nie tak z tangentami?
    modelLoader.m_uvSize = 3;
    modelLoader.open(path + ".dae", std::move(assets::layerSearch(assets::getAlbedoArray("Materials"))));
    if(not modelLoader.good){
        error("Unable to load environment");
        return;
    }

    Yaml yaml(path + ".yml");
    for(auto& it : yaml["Objects"]) loadObject(it, modelLoader);
    if(yaml.has("LightSources")) for(auto& it : yaml["LightSources"]) loadLightSource(it);
    vao = modelLoader.build();

    const Yaml &lamps = yaml["LightSources"];
}

void Environment::loadObject(const Yaml &thing, ModelLoader& modelLoader){
    EnviroEntity e {};
    e.name = thing["Name"].string();

    auto x = thing["Position"]["X"].vec4();
    auto y = thing["Position"]["Y"].vec4();
    auto z = thing["Position"]["Z"].vec4();
    auto w = thing["Position"]["W"].vec31();

    e.physics.position = w;
    e.physics.transform = glm::mat4(x,y,z,w);

    loadMesh(modelLoader, e, thing);

    if(thing["isPhysical"].boolean() and thing["Colliders"].string() != "none"){
        auto colliders = modelLoader.loadCompoundMeshes({thing["Colliders"].strings()});
        // e.physics.rgBody = physics->createRigidBody(0, convert(obj["Quaternion"].quat(), e.physics.position), createCompoundMesh(colliders, nullptr));
    }
    e.id = entities.size();
    entities.push_back(e);
    SceneObject object{Type::Enviro, SceneObject::nextID(), nullptr, e.id};
    if(e.physics.rgBody) e.physics.rgBody->setUserIndex(object.ID);

    graph.insertObject(object, e.physics.position);
}

void Environment::loadLightSource(const Yaml &thing){
    auto &&l = lightSources.emplace(thing["Type"].string());
    l->m_energy = thing["Energy"].number();
    l->m_fallof = thing["Falloff_distance"].number();
    l->m_color = thing["Color"].vec4();
    // l->setTransform(lamp["Position"].vec4(), lamp["Quaternion"].quat());
}

// TODO: zrobic tak by convex był pojedynczym modelem, rozbitym przy pomocy materialow, przypiety jako dziecko wlasciwego modelu
// TODO: fajnie by bylo by dało się reusowac modele(export object instances, rozmieszcac przy pomocy Alt+D, uzywac nazwy mesha nie obiektu)
// duplikacje?
void Environment::loadMesh(ModelLoader &modelLoader, EnviroEntity &e, const Yaml &yaml){
    e.graphic.mesh = modelLoader.beginMesh();
    modelLoader.load(yaml["Models"].strings());
    modelLoader.endMesh(e.graphic.mesh);

    if(yaml["isPhysical"].boolean()){
        for(auto &collider : yaml["Colliders"]){
            // load bullet rgBody
        }
    }

}
/** example:
- Name: Rock
  Mesh: Rock.mesh
  Position: [20, 25, -3]
  Convex: Rock.convex
*/
btRigidBody* Environment::createRgBody(const Yaml &bodyConf){
    btCollisionShape *shape = nullptr;
    // jak tam bylo z raycastem na compound mesh? kto byl logowany jako owner? ptr ustawiony w rgBody, nie?

    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(bodyConf["Position"].btVec());

    return physics.createRigidBody(0, tr, shape);
}

void Environment::insertObjectToQt(u32 id){
    auto &e = getObject(id);

    glm::vec4 position = e.physics.position;
    float radius = 10;

    // QT.findTouchedNodes(position, radius).objects.push_back(id);
}
