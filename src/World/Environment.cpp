#include "Environment.hpp"
#include "common.hpp"
#include "Logging.hpp"
#include "ResourceLoader.hpp"
#include "LightSource.hpp"
#include "ModelLoader.hpp"
#include "GPUResources.hpp"
#include "PerfTimers.hpp"
#include "PhysicalWorld.hpp"
#include "Yaml.hpp"
#include "Utils.hpp"

void Environment::load(const std::string &dirPath){
    CPU_SCOPE_TIMER("Environment::load");

    ModelLoader modelLoader;
    modelLoader.loadTangents = true;
    modelLoader.open(dirPath + "/map.dae", std::move(assets::layerSearch(assets::getAlbedoArray("Materials"))));
    if(not modelLoader.good){
        error("Unable to load environment");
        return;
    }

    Yaml yaml(dirPath + "/map.yml");

    for(auto &obj : yaml["Objects"]){
        EnviroEntity e {};
        e.physics.position = obj["Position"].vec31();
        e.physics.transform = glm::translate(e.physics.position.xyz()) * glm::toMat4(obj["Quaternion"].quat());
        loadMesh(modelLoader, e, obj);
        if(obj.has("Colliders")){
            if(obj["Colliders"].size() != 1){
                error("Unsupported number of collisders");
            }
            auto colliders = modelLoader.loadCompoundMeshes({obj["Colliders"][0]["Mesh"].string()});
            e.physics.rgBody = physics->createRigidBody(0, convert(obj["Quaternion"].quat(), e.physics.position), createCompoundMesh(colliders, nullptr));
        }
        e.id = entities.size();
        entities.push_back(e);
        Object object{Type::Enviro, Object::nextID(), nullptr, e.id};
        if(e.physics.rgBody) e.physics.rgBody->setUserIndex(object.ID);
        graph.objects[object.ID] = object;
    }
    vao = modelLoader.build();
    return;
    const Yaml &lamps = yaml["LightSources"];
    for(auto &lamp : lamps){
        auto &&l = lightSources.emplace(lamp["Type"].string());
        l->m_energy = lamp["Energy"].number();
        l->m_fallof = lamp["Falloff_distance"].number();
        l->m_color = lamp["Color"].vec4();
        l->setTransform(lamp["Position"].vec4(), lamp["Quaternion"].quat());
    }
}

// TODO: zrobic tak by convex był pojedynczym modelem, rozbitym przy pomocy materialow, przypiety jako dziecko wlasciwego modelu
// TODO: fajnie by bylo by dało się reusowac modele(export object instances, rozmieszcac przy pomocy Alt+D, uzywac nazwy mesha nie obiektu)
// duplikacje?
void Environment::loadMesh(ModelLoader &modelLoader, EnviroEntity &e, const Yaml &yaml){
    e.graphic.mesh = modelLoader.beginMesh();
    modelLoader.load(yaml["Mesh"].string());
    modelLoader.endMesh(e.graphic.mesh);

    if(yaml["isCollider"].boolean()){
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
    btCollisionShape *shape;
    // jak tam bylo z raycastem na compound mesh? kto byl logowany jako owner? ptr ustawiony w rgBody, nie?

    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(bodyConf["Position"].btVec());

    return physics->createRigidBody(0, tr, shape);
}

void Environment::insertObjectToQt(u32 id){
    auto &e = getObject(id);

    glm::vec4 position = e.physics.position;
    float radius = 10;

    // QT.findTouchedNodes(position, radius).objects.push_back(id);
}
