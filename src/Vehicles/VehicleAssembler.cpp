#include "core.hpp"
#include "VehicleAssembler.hpp"
#include "Assets.hpp"
#include "DroneLikeControl.hpp"
#include "IModule.hpp"
#include "Joint.hpp"
#include "ModelLoader.hpp"
#include "PhysicalWorld.hpp"
#include "Player.hpp"
#include "camera-factory.hpp"

class CameraInObjectSpace : public camera::Controller
{
private:
    IModule& m_module;

public:
    template<typename... Args>
    CameraInObjectSpace(IModule& module, Args&&... args)
        : camera::Controller(module.getTransform(), std::forward<Args>(args)...), m_module(module) {
        setup.isFreecam = false;
        setup.inLocalSpace = false;
        setup.inLocalSpaceRotationOnly = false;
        setup.inLocalSpacePlane = false;
    }

    void update(float dt) override {
        camera::Controller::update(m_module.getTransform(), dt);
    }
};

class CameraInObjectsTurretSpace : public camera::Controller
{
private:
    IModule& m_module;

public:
    template<typename... Args>
    CameraInObjectsTurretSpace(IModule& module, Args&&... args)
        : camera::Controller(module.getTransform(), std::forward<Args>(args)...), m_module(module) {
        setup.isFreecam = false;
        setup.inLocalSpace = true;
        setup.inLocalSpaceRotationOnly = false;
        setup.inLocalSpacePlane = false;
    }

    void update(float dt) override {
        camera::Controller::update(m_module.getTransform(), dt);
    }
};

glm::mat4 matrixFromYaml(const Yaml& params) {
    return glm::mat4(params["X"].vec30(), params["Y"].vec30(), params["Z"].vec30(), params["W"].vec31());
}

VehicleAssembler::VehicleAssembler(const std::string& configName, PhysicalWorld& physics, camera::Factory& camFactory)
    : m_configName(configName),
      m_modelLoader(std::make_shared<ModelLoader<VertexWithMaterialDataAndBones>>()),
      m_physics(physics),
      m_vehicle(std::make_shared<Vehicle>(physics)),
      m_moduleFactory(*m_vehicle, m_physics, {}),
      m_camFactory(camFactory) {}

// * builds common part of model, every specific should be done by inheritances
std::shared_ptr<Vehicle> VehicleAssembler::build(const glm::mat4& onPosition) {
    console_prefix(m_configName + " Assembly");

    openModelFile();
    initializeVehicle(onPosition);
    assemblyVehicleModules();
    finishAssembly(onPosition);

    return m_vehicle;
}

void VehicleAssembler::openModelFile() {
    console.log("Starting assembly of", m_configName);
    m_config = Yaml(resPath + "models/" + m_configName + ".yml");

    // m_modelLoader->tangents = 3; // TODO:
    m_modelLoader->materials = m_config["Materials"];
    m_modelLoader->defaults.uvComponents = 3;
    m_modelLoader->defaults.vertex4comonent = 0u;
    m_modelLoader->defaults.roughness = 0.5f;
    m_modelLoader->defaults.metallic = 0.0f;
    m_modelLoader->defaults.reflectivity = 0.04f;
    m_modelLoader->open(resPath + "models/" + m_configName + ".dae",
                        assets::layerSearch(assets::getAlbedoArray("Materials")));
}

void VehicleAssembler::initializeVehicle(const glm::mat4& onPosition) {
    m_vehicle->fireControlUnit = std::make_unique<FireControlSystem>();
    m_vehicle->compound = new btCompoundShape();
    m_skinnedMesh = std::make_shared<SkinnedMesh>();
    m_skinnedMesh->mesh = m_modelLoader->beginMesh();
}

void VehicleAssembler::assemblyVehicleModules() {
    auto base = m_config["Modules"].find([](auto& it) { return it["Class"].string() == "Hull"; });

    if(not base) {
        console.error("No module of class Hull to begin with!");
        return;
    }

    assemblyModuleAndItsChildren(nullptr, *base);
}

void VehicleAssembler::finishAssembly(const glm::mat4& onPosition) {
    // * put model to GPU
    m_modelLoader->endMesh(m_skinnedMesh->mesh);
    m_skinnedMesh->vao = m_modelLoader->build();
    m_vehicle->compound->recalculateLocalAabb();
    m_vehicle->graphics.entitiesToDraw.push_back(std::move(m_skinnedMesh));

    // m_vehicle->modulesToUpdateInsidePhysicsStep.push_back(m_vehicle->driveSystem);
    m_physics.m_dynamicsWorld->addAction(m_vehicle.get());

    buildRigidBody(onPosition);
    m_vehicle->vehicleControlUnit = std::make_unique<DroneLikeControl>(m_physics, *m_vehicle, convert(onPosition[3]));
}

void VehicleAssembler::assemblyModuleAndItsChildren(IModule* parent, const Yaml& params) {
    auto module = m_moduleFactory.createModule(params, parent);
    if(not module) {
        console.error("failed to create module:", params["Name"].string());
        return;
    }

    module->name += ": " + params["Name"].string();
    m_vehicle->modules.push_back(module);
    module->localTransform = matrixFromYaml(params["Relative Position"]);

    setDecals(*module, params);
    setMarkers(*module, params);
    // setArmor(*module, params);
    setVisual(*module, params);
    if(parent)
        setServoAndMotionLimits(*module);
    setPhysical(*module, params);
    if(params.has("Cameras"))
        attachCameras(*module, params["Cameras"]);

    params.for_each("Attached Modules", [&](const Yaml& yml) {
        for(auto moduleName : yml["Modules"].strings())
            assemblyModuleAndItsChildren(module.get(), m_config["Modules"][moduleName]);
    });
}

void VehicleAssembler::buildRigidBody(const glm::mat4& onPosition) {
    if(not m_hasAnyPhysicalPart)
        console.error("Vehicle doesn't have any physical part!");

    btTransform tr(convert(onPosition));

    // float mass = descriptionForModules["mass"].number();
    float mass = 20;
    m_vehicle->rgBody = m_physics.createRigidBody(mass, tr, m_vehicle->compound);
    // vehicleEquipment.rgBody->setUserPointer((void *)(&vehicleEquipment));

    m_vehicle->btTrans = tr;
    m_vehicle->glTrans = onPosition;
    m_vehicle->rgBody->setDamping(0.6f, 0.6f);
    m_vehicle->rgBody->setActivationState(DISABLE_DEACTIVATION);
}

void VehicleAssembler::setDecals(IModule& module, const Yaml& cfg) {
    if(not cfg.has("Decals"))
        return;
    auto& decals = cfg["Decals"];

    for(auto& decal : decals) {
        // module.decals.emplace_back();
        // module.decals.back().layer = assets::getAlbedoArray("Decals").find(decal["Layer"].string());
        // glm::mat4 tr(
        //         decal["LocX"].vec30(),
        //         glm::normalize(cross(decal["LocX"].vec30(), decal["LocZ"].vec30())),
        //         decal["LocZ"].vec30(),
        //         glm::vec4(0,0,0,1)
        //     );

        // module.decals.back().transform = glm::translate(-decal["Position"].vec3()) * tr * glm::scale(decal["Scale"].vec3()*0.9f);
    }
}

void VehicleAssembler::setMarkers(IModule& module, const Yaml& cfg) {
    if(not cfg.has("Markers"))
        return;
    auto& markers = cfg["Markers"];
    for(auto& marker : markers) {
        // else if("EndOfBarrel"s == marker["Type"].string() and module.type == ModuleType::Cannon){
        // Cannon& gun = module;
        // gun->endOf = marker["Position"].vec31();
        // }
        // else if("Light"s == marker["Type"].string()){
        // modulesToAddLater.emplace_back(m_moduleFactory.createHeadlight(marker));
        // }
    }
}

void VehicleAssembler::attachCameras(IModule& module, const Yaml& names) {
    for(const auto& name : names) {
        const auto& params = m_config["Cameras"][name.string()];
        // todo: load full camera transformation matrix
        auto camera = m_camFactory.create<CameraInObjectSpace>(module, matrixFromYaml(params["Relative Position"]));
        camera->fov = params["Angle"].number();
        camera->inertia = params["Inertia"].number();
        camera->recalucuateProjectionMatrix();
        camera->update(0.f);

        m_vehicle->cameras.add(camera);
        console.log("Camera:", name.string());
    }
}

// * create visual representation of module
// * combine few models into one, despite that models are split by material(in assimp), they can be splitted in editor(for usefulness)
void VehicleAssembler::setVisual(IModule& module, const Yaml& cfg) {
    if(not cfg.has("Models")) {
        module.moduleVisualUpdater = std::make_unique<NullModuleVisualUpdater>();
        console.log("No visuals");
        return;
    }
    auto models = cfg["Models"].strings();

    m_skinnedMesh->bones.push_back(identityMatrix);
    module.moduleVisualUpdater = std::make_unique<ModuleVisualUpdater>(m_skinnedMesh->bones, m_boneMatrixIndex);
    // module.moduleVisualUpdater->setTransform(identityMatrix);
    module.moduleVisualUpdater->setTransform(module.getParentTransform() * module.localTransform);

    m_modelLoader->setBoneIndex(m_modelLoader->loadOnly(models), m_boneMatrixIndex);

    m_boneMatrixIndex++;
}

// * creates connection between parent and child module, usually this connection is updated by child
// * can have different number of dof
// * lack of limits means that connection is rigid
void VehicleAssembler::setServoAndMotionLimits(IModule& module) {
    // auto joint = createJoint(connectionParams, fromJointToOrigin);
    // module.setJoint(std::move(joint));
}

// * when module has rigidBody created
void VehicleAssembler::setPhysical(IModule& module, const Yaml& cfg) {
    if(not cfg.has("Physical"))
        return;
    m_hasAnyPhysicalPart = true;

    auto localTransformation = module.getParentLocalTransform() * module.localTransform;

    if(cfg["Physical"].has("CollisionModels") and cfg["Physical"]["CollisionModels"] != "none") {
        auto meshes = m_modelLoader->loadConvexMeshes(cfg["Physical"]["CollisionModels"].strings());
        addToCompound(createCompoundShape(meshes, (void*)(&module)), localTransformation, (void*)(&module));
    }
    else {
        // TODO: create dummy collision model
        addToCompound(new btBoxShape(btVector3(2, 2, 2)), localTransformation, (void*)(&module));
    }
    module.moduleCompoundUpdater = std::make_unique<ModuleCompoundUpdater>(m_vehicle->compound, m_compoundIndex++);
}

void VehicleAssembler::setArmor(IModule& module, const Yaml& cfg) {}

void VehicleAssembler::addToCompound(btCollisionShape* collShape, const glm::mat4& transform, void* owner) {
    btTransform localTrans = convert(transform);
    collShape->setUserPointer(owner);
    m_vehicle->compound->addChildShape(localTrans, collShape);
}
