#include "gl_core_4_5.hpp"
#include "Camera.hpp"
#include "common.hpp"
#include "Scene.hpp"
#include "ResourceLoader.hpp"
#include "Yaml.hpp"
#include "PerfTimers.hpp"
#include "PhysicsWorld.hpp"

#include "Terrain.hpp"
#include "Sun.hpp"
#include "Starfield.hpp"
#include "Grass.hpp"
#include "Environment.hpp"
#include "Atmosphere.hpp"
#include "GeoTimePosition.hpp"
#include "Foliage.hpp"
#include "LightSource.hpp"

namespace graphic {void renderTopViewOfTerrain(Scene &scene);}

Scene::~Scene(){
    log("~Scene");
}

bool Scene::load(const std::string &name, Yaml &cfg){
    CPU_SCOPE_TIMER("Scene::load");

    log("---------------");
    if(not physics){
        error("no physics");
    }

    Yaml settings("../SceneSettings.yml");

    geoTimePosition = std::make_shared<GeoTimePosition>(settings["GeoTime"]);
    sun = std::make_shared<Sun>(settings["Sun"], *geoTimePosition);
    starfield = std::make_shared<Starfield>();
    starfield->regenerate();
    atmosphere = std::make_shared<Atmosphere>(settings["Atmosphere"], *geoTimePosition);

    graph = std::make_shared<SceneGraph>(*physics);
    graph->loadMap(cfg["Map"]["Dir"].string());

    environment = std::make_shared<Environment>(*graph, physics);
    environment->load(cfg["Map"]["Dir"].string());
    // terrain = std::make_shared<Terrain>(*quadTree);
    // terrain->create(cfg["Map"]);
    // terrain->uploadTexture();
    //
    // quadTree->buildQTNodes(*terrain);
    // terrain->finalize();
    // quadTree->recalculateNodeZPosition();
    //
    // grass = std::make_shared<Grass>(*quadTree);
    // grass->loadData(loader, cfg["Res"]["Grass"]["Common"]);
    //
    // foliage = std::make_shared<Foliage>(*quadTree, *physics);
    // foliage->load(Global::m_resources["Foliage"]);
    //
    // grass->initVBO();
    physics->update(0.1);
    // Global::sampleGround = [this](glm::vec4 p) -> SampleResult {return sample(p);};

    // graphic::renderTopViewOfTerrain(*this);

    return true;
}

void Scene::update(float dt, Camera &camera){
    CPU_SCOPE_TIMER("Scene::update");
    const Frustum &frustum = camera.getFrustum();
    if(geoTimePosition) geoTimePosition->update(dt*10);
    if(graph) graph->cullCells(frustum);

    // if(grass) grass->update(camera.position);
    // if(foliage) foliage->update(camera.position);
    if(sun) sun->update(*atmosphere);
    if(atmosphere) atmosphere->update(sun->getVector());
}
