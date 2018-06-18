#include "core.hpp"
#include "gl_core_4_5.hpp"

#include "Atmosphere.hpp"
#include "Camera.hpp"
#include "Environment.hpp"
#include "Foliage.hpp"
#include "GeoTimePosition.hpp"
#include "Grass.hpp"
#include "LightSource.hpp"
#include "PerfTimers.hpp"
#include "PhysicalWorld.hpp"
#include "ResourceLoader.hpp"
#include "Starfield.hpp"
#include "Sun.hpp"
#include "Terrain.hpp"
#include "Yaml.hpp"
#include "Scene.hpp"

namespace graphic {void renderTopViewOfTerrain(Scene &scene);}

Scene::Scene(PhysicalWorld &physics) : physics(physics){}
Scene::~Scene(){
    log("~Scene");
}

bool Scene::load(const std::string &sceneName){
    CPU_SCOPE_TIMER("Scene::load");

    Yaml settings(resPath + "scenes/" + sceneName + "/SceneSettings.yml");

    geoTimePosition = std::make_unique<GeoTimePosition>(settings["GeoTime"]);
    sun = std::make_unique<Sun>(settings["Sun"], *geoTimePosition);
    starfield = std::make_unique<Starfield>();
    starfield->regenerate();
    atmosphere = std::make_unique<Atmosphere>(settings["Atmosphere"], *geoTimePosition);

    graph = std::make_unique<SceneGraph>(physics);
    // graph->loadMap(cfg["Map"]["Dir"].string());

    environment = std::make_unique<Environment>(*graph, physics);
    environment->load(sceneName);
    // terrain = std::make_unique<Terrain>(*quadTree);
    // terrain->create(cfg["Map"]);
    // terrain->uploadTexture();
    //
    // quadTree->buildQTNodes(*terrain);
    // terrain->finalize();
    // quadTree->recalculateNodeZPosition();
    //
    // grass = std::make_unique<Grass>(*quadTree);
    // grass->loadData(loader, cfg["Res"]["Grass"]["Common"]);
    //
    // foliage = std::make_unique<Foliage>(*quadTree, *physics);
    // foliage->load(Global::m_resources["Foliage"]);
    //
    // grass->initVBO();
    // physics.update(0.1);
    // Global::sampleGround = [this](glm::vec4 p) -> SampleResult {return sample(p);};

    // graphic::renderTopViewOfTerrain(*this);

    return true;
}

void Scene::update(float dt, Camera &camera){
    CPU_SCOPE_TIMER("Scene::update");
    const Frustum &frustum = camera.getFrustum();
    if(geoTimePosition) geoTimePosition->update(dt*10);
    if(graph) graph->cullCells(frustum);

    // if(grass) grass->update(camera.position());
    // if(foliage) foliage->update(camera.position());
    if(sun) sun->update(*atmosphere);
    if(atmosphere) atmosphere->update(sun->getVector());
}
