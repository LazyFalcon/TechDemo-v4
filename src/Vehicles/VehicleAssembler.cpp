#include "core.hpp"

#include "Assets.hpp"
#include "CameraControllerFactory.hpp"
#include "DummyDriveSystem.hpp"
#include "IModule.hpp"
#include "Joint.hpp"
#include "ModelLoader.hpp"
#include "PhysicalWorld.hpp"
#include "Player.hpp"
#include "VehicleAssembler.hpp"

#define logFunc() log(__FUNCTION__, ":", __LINE__);

VehicleAssembler::VehicleAssembler(const std::string& configName, Player& player, PhysicalWorld& physics, CameraControllerFactory& camFactory) :
    m_configName(configName),
    m_modelLoader(std::make_shared<ModelLoader<VertexWithMaterialDataAndBones>>()),
    m_physics(physics),
    m_vehicleEq(std::make_shared<VehicleEquipment>(physics)),
    m_moduleFactory(*m_vehicleEq, m_physics, {}),
    m_player(player),
    m_camFactory(camFactory)
    {}

void VehicleAssembler::openModelFile(){
    // m_modelLoader->tangents = 3; // TODO:
    m_modelLoader->defaults.uvComponents = 3;
    m_modelLoader->defaults.vertex4comonent = 0u;
    m_modelLoader->defaults.roughness = 0.5f;
    m_modelLoader->defaults.metallic = 0.0f;
    m_modelLoader->defaults.reflectivity = 0.04f;
    m_modelLoader->open(resPath + "models/" + m_configName + ".dae", assets::layerSearch(assets::getAlbedoArray("Materials")));

    m_config = Yaml(resPath + "models/" + m_configName + ".yml");
}

// * builds common part of model, every specific should be done by inheritances
void VehicleAssembler::build(const glm::mat4& onPosition){
    openModelFile();

    m_skinnedMesh = std::make_shared<SkinnedMesh>();
    m_vehicleEq->compound = new btCompoundShape();
    m_skinnedMesh->mesh = m_modelLoader->beginMesh();

    makeModulesRecursively(m_config["Model"], {}, nullptr);

    // * put model to GPU
    m_modelLoader->endMesh(m_skinnedMesh->mesh);
    m_skinnedMesh->vao = m_modelLoader->build();
    m_vehicleEq->compound->recalculateLocalAabb();
    m_player.graphics.entitiesToDraw.push_back(std::move(m_skinnedMesh));

    m_vehicleEq->driveSystem = std::make_shared<DummyDriveSystem>(*m_vehicleEq, convert(onPosition[3]));
    m_vehicleEq->modulesToUpdateInsidePhysicsStep.push_back(m_vehicleEq->driveSystem);
    m_physics.m_dynamicsWorld->addAction(m_vehicleEq.get());

    for(auto& it : m_vehicleEq->modules){
        it->init();
    }

    m_player.setEq(m_vehicleEq);
    buildRigidBody(onPosition);
    // m_vehicleEq->cameras[0]->focus();
}

void VehicleAssembler::buildRigidBody(const glm::mat4& onPosition){
    btTransform tr(convert(onPosition));
    // tr.setIdentity();
    // tr.setOrigin(convert(onPosition[3]));

    // float mass = descriptionForModules["mass"].number();
    float mass = 20;
    m_vehicleEq->rgBody = m_physics.createRigidBody(mass, tr, m_vehicleEq->compound);
    // vehicleEquipment.rgBody->setUserPointer((void *)(&vehicleEquipment));

    m_vehicleEq->rgBody->setDamping(0.2f, 0.2f);
    m_vehicleEq->rgBody->setActivationState(DISABLE_DEACTIVATION);
}

void VehicleAssembler::makeModulesRecursively(const Yaml& cfg, const Yaml& connectionProps, IModule *parentModule){
    if(not cfg["Active"].boolean()) return;

    std::string identifier = cfg["Identifier"].string();
    std::string modelName = cfg["Name"].string();

    log("Processing:", modelName);

    auto module = m_moduleFactory.createModule(cfg);
    if(not module){
        error("failed to create module:", modelName);
        return;
    }
    module->parent = parentModule;
    module->name = modelName;
    module->joint = parentModule==nullptr ? std::make_shared<Joint>() : createJoint(connectionProps, cfg["FromParentToOrigin"].vec30());
    m_vehicleEq->modules.push_back(module);

    setDecals(*module, cfg);
    setMarkers(*module, cfg);
    setVisual(*module, cfg);
    setConnection(*module, cfg);
    setPhysical(*module, cfg);
    // setArmor(*module, cfg);

    if(cfg.has("Connector")) for(auto& connector : cfg["Connector"]){
        if(connector.has("Pinned")) for(auto& pinned : connector["Pinned"]){
            makeModulesRecursively(pinned, connector, module.get());
        }
    }
    return;
}

void VehicleAssembler::setDecals(IModule& module, const Yaml& cfg){
    if(not cfg.has("Decals")) return;
    auto& decals = cfg["Decals"];

    for(auto& decal : decals){
        module.decals.emplace_back();
        module.decals.back().layer = assets::getAlbedoArray("Decals").find(decal["Layer"].string());
        glm::mat4 tr(
                decal["LocX"].vec30(),
                glm::normalize(cross(decal["LocX"].vec30(), decal["LocZ"].vec30())),
                decal["LocZ"].vec30(),
                glm::vec4(0,0,0,1)
            );

        module.decals.back().transform = glm::translate(-decal["Position"].vec3()) * tr * glm::scale(decal["Scale"].vec3()*0.9f);
    }
}

void VehicleAssembler::setMarkers(IModule& module, const Yaml& cfg){
    if(not cfg.has("Markers")) return;
    auto& markers = cfg["Markers"];
    for(auto& marker : markers){
        glm::mat4 mat(
            marker["X"].vec30(),
            marker["Y"].vec30(),
            marker["Z"].vec30(),
            marker["W"].vec31()
        );
        if("Camera"s == marker["Type"].string()){
            auto camera = createModuleFollower(&module, marker["Mode"].string(), marker["W"].vec3(), mat);
            camera->offset = marker["Offset"].vec31();
            camera->fov = marker["FOV"].number()*toRad;
            camera->inertia = marker["Inertia"].number();
            camera->recalucuateProjectionMatrix();
            camera->evaluate();

            m_vehicleEq->cameras.push_back(camera);
        }
        else if("EndOfBarrel"s == marker["Type"].string() and module.type == ModuleType::Cannon){
            // Cannon& gun = module;
            // gun->endOf = marker["Position"].vec31();
        }
        else if("Light"s == marker["Type"].string()){
            // modulesToAddLater.emplace_back(m_moduleFactory.createHeadlight(marker));
        }
    }
}

// * create visual representation of module
// * combine few models into one, despite that models are split by material(in assimp), they can be splitted in editor(for usefulness)
void VehicleAssembler::setVisual(IModule& module, const Yaml& cfg){
    if(not cfg.has("Models")){
        module.moduleVisualUpdater = std::make_unique<NullModuleVisualUpdater>();
        return;
    }
    auto& models = cfg["Models"].strings();

    m_skinnedMesh->bones.push_back(identityMatrix);
    module.moduleVisualUpdater = std::make_unique<ModuleVisualUpdater>(m_skinnedMesh->bones, m_boneMatrixIndex);
    module.moduleVisualUpdater->setTransform(identityMatrix);

    m_modelLoader->setBoneIndex(m_modelLoader->loadOnly(models), m_boneMatrixIndex);

    m_boneMatrixIndex++;
}

// * creates connection between parent and child module, usually this connection is updated by child
// * can have different number of dof
// * lack of limits means that connection is rigid
void VehicleAssembler::setConnection(IModule& module, const Yaml& cfg){
    glm::mat4 tr = module.joint->getTransform();
    module.transform(tr);
}

// * when module has rigidBody created
void VehicleAssembler::setPhysical(IModule& module, const Yaml& cfg){
    if(not cfg.has("Physical")) return;

    auto tr = module.getParentTransform() * module.joint->getTransform();

    // glm::vec4 cnvPos = glm::vec4(module.joint.toPivot + module.joint.toOrigin);
    auto localTransformation = module.joint->getTransform();

    if(cfg["Physical"].has("CollisionModels") and cfg["Physical"]["CollisionModels"] != "none"){
        auto meshes = m_modelLoader->loadConvexMeshes(cfg["Physical"]["CollisionModels"].strings());

        addToCompound(createCompoundShape(meshes, (void*)(&module)), localTransformation, (void*)(&module));
        module.moduleCompoundUpdater = std::make_unique<ModuleCompoundUpdater>(m_vehicleEq->compound, m_compoundIndex++);
    }
    else {
        // TODO: create dummy collision model

        addToCompound(new btBoxShape(btVector3(2,2,2)), localTransformation, (void*)(&module));
        module.moduleCompoundUpdater = std::make_unique<ModuleCompoundUpdater>(m_vehicleEq->compound, m_compoundIndex++);
    }
}

void VehicleAssembler::setArmor(IModule& module, const Yaml& cfg){}

void VehicleAssembler::addToCompound(btCollisionShape* collShape, const glm::mat4& transform, void* owner){
    btTransform localTrans = convert(transform);
    collShape->setUserPointer(owner);
    m_vehicleEq->compound->addChildShape(localTrans, collShape);
}


std::shared_ptr<CameraController> VehicleAssembler::createModuleFollower(IModule *module, const std::string& type, glm::vec3 position, const glm::mat4& mat){
    if(type == "CopyPosition"){
        return m_camFactory.create<ModuleFollower<CopyOnlyPosition>>(module, position, mat);
    }
    else if(type == "CopyTransform"){
        return m_camFactory.create<ModuleFollower<CopyTransform>>(module, position, mat);
    }
    else if(type == "Free"){
        return m_camFactory.create<FreeCamController>(mat);
    }
    return nullptr;
}
