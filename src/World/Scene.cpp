#include "core.hpp"
#include "gl_core_4_5.hpp"
#include "Scene.hpp"
#include "Atmosphere.hpp"
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
#include "camera-controller.hpp"
#include "camera-data.hpp"
#include "camera-factory.hpp"
#include "visulas-collected.hpp"

namespace graphic
{
void renderTopViewOfTerrain(Scene& scene);
}

Scene::Scene(PhysicalWorld& physics, camera::Factory& camFactory) : physics(physics), camFactory(camFactory) {}
Scene::~Scene() {
    console.log("~Scene");
}

bool Scene::load(const std::string& sceneName) {
    CPU_SCOPE_TIMER("Scene::load");

    Yaml settings(resPath + "scenes/" + sceneName + "/SceneSettings.yml");

    geoTimePosition = std::make_unique<GeoTimePosition>(settings["GeoTime"]);
    // ingameClock = std::make_unique<InGameClock>(geoTimePosition);
    sun = std::make_unique<Sun>(settings["Sun"], *geoTimePosition);
    starfield = std::make_unique<Starfield>();
    starfield->regenerate();
    atmosphere = std::make_unique<Atmosphere>(settings["Atmosphere"], *geoTimePosition);

    graph = std::make_unique<SceneGraph>(physics);
    graph->initAndLoadMap(settings["Scene"]);

    Yaml sceneYaml(resPath + "scenes/" + sceneName + "/" + sceneName + ".yml");

    environment = std::make_unique<Environment>(*graph, physics);
    environment->load(sceneName, sceneYaml);

    // todo: loadTerrainCellsToGraph(sceneYaml);
    extractSpawnPoints(sceneYaml);
    extractCameras(sceneYaml);
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

void Scene::extractSpawnPoints(const Yaml& yaml) {
    if(yaml.has("Markers"))
        for(auto& it : yaml["Markers"]) {
            if(it["Type"] == "SpawnPoint") {
                auto& p = it["Position"];
                spawnPoints.push_back(
                    {it["Name"].string(), glm::mat4(p["X"].vec30(), p["Y"].vec30(), p["Z"].vec30(), p["W"].vec31())});
            }
        }
}
void Scene::extractCameras(const Yaml& yaml) {
    if(yaml.has("Cameras"))
        for(auto& it : yaml["Cameras"]) {
            auto x = it["Position"]["X"].vec30();
            auto y = it["Position"]["Y"].vec30();
            auto z = it["Position"]["Z"].vec30();
            auto w = it["Position"]["W"].vec31();
            glm::mat4 mat(x, y, z, w);
            // todo: fill the rest of params!
            auto cam = camFactory.create<camera::Controller>(mat);
            freeCams.add(cam);

            cam->setup.inLocalSpace = false;
            cam->setup.addRotationToTarget = false;
            cam->setup.addInclinationToTarget = false;
            cam->setup.zoomMode = camera::OFFSET;
            cam->userPointerMode = camera::PointerMode::Centered;
        }
    else
        console.error("There is no free camera defined in scene");
    freeCams.focus();
}

glm::vec4 Scene::getSceneDimensions() {
    return graph->getDimensions();
}

void Scene::updateWorld(float dt) {
    // todo: ingameClock->update(dt);
    // todo: a jakby podawać obiektom ref do struktur które mają wypełnić danymi do renderowania? niezłe?
    sun->update(*atmosphere);
    atmosphere->update(sun->direction);
    // todo: wind->update();
    storeMainLight();
}
void Scene::updateNonPlayableObjects(float dt, camera::Camera& camera) {
    environment->update(dt);
    // todo: if(grass) grass->update(camera.position());
    // todo: if(foliage) foliage->update(camera.position());

    // todo: updateShadowCasters();
}
void Scene::updateLightsAndShadows(float dt, camera::Camera& camera) {}
void Scene::collectObjectForRendering(float dt, camera::Camera& camera) {
    CPU_SCOPE_TIMER("Scene::culling");
    const camera::Frustum& frustum = camera.getFrustum();
    graph->cullCells(frustum);
}

void Scene::storeMainLight() {
    if(isDay()) {
        visuals::collected.mainLight.direction = sun->direction;
        visuals::collected.mainLight.color = sun->color;
        visuals::collected.mainLight.colorTemperature = 6000.f;
        visuals::collected.mainLight.power = sun->power;
    }
}
