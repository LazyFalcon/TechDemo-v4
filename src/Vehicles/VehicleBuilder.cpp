#include "core.hpp"
#include "Assets.hpp"
#include "CameraControllerFactory.hpp"
#include "IModule.hpp"
#include "PhysicalWorld.hpp"
#include "Player.hpp"
#include "VehicleBuilder.hpp"

#define logFunc() log(__FUNCTION__, ":", __LINE__);

VehicleBuilder::VehicleBuilder(const std::string& configName, Player& player, PhysicalWorld& physicalWorld, CameraControllerFactory& camFactory) :
    m_configName(configName),
    m_modelLoader(std::make_unique<ModelLoader>()),
    m_physicalWorld(physicalWorld),
    m_moduleFactory(player.eq(), m_physicalWorld, {}),
    m_player(player),
    m_camFactory(camFactory)
    {}

void VehicleBuilder::openModelFile(){
    // m_modelLoader->tangents = 3; // TODO:
    m_modelLoader->defaults.uvComponents = 3;
    m_modelLoader->defautls.vertex4comonent = 0u;
    m_modelLoader->defautls.roughness = 0.5f;
    m_modelLoader->defautls.metallic = 0.0f;
    m_modelLoader->defautls.reflectivity = 0.04f;
    m_modelLoader->open(resPath + "models/" + m_configName + ".dae", assets::layerSearch(assets::getAlbedoArray("Materials")));

    m_config = Yaml(resPath + "models/" + m_configName + ".yml");
}

// * builds common part of model, every specific should be done by inheritances
void VehicleBuilder::build(){
    openModelFile();

    m_skinnedMesh = std::make_shared<SkinnedMesh>();

    // // m_player.eq().compound = new btCompoundShape();
    m_skinnedMesh->mesh = m_modelLoader->beginMesh();

    Joint dummyJoint;
    makeModulesRecursively(m_config["Model"], dummyJoint, nullptr);
    // * collision models are not needed now
    // //buildRigidBody();

    // * put model to GPU
    m_modelLoader->endMesh(m_skinnedMesh->mesh);
    m_skinnedMesh->vao = m_modelLoader->build();
    // vehicleEquipment.compound->recalculateLocalAabb();
    m_player.graphics.entitiesToDraw.push_back(std::move(m_skinnedMesh));

    for(auto& it : m_player.eq().modules){
        it->init();
    }

    // m_player.eq().cameras[0]->focus();
}

void VehicleBuilder::makeModulesRecursively(const Yaml& cfg, Joint& connectorJoint, IModule *parentModule){
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
    m_player.eq().modules.push_back(module);

    setDecals(*module, cfg);
    setMarkers(*module, cfg);
    setVisual(*module, cfg);
    setConnection(*module, cfg, connectorJoint);
    // setPhysical(*module, cfg);
    // setArmor(*module, cfg);

    if(cfg.has("Connector")) for(auto& connector : cfg["Connector"]){
        auto x = connector["X"].vec30();
        auto y = connector["Y"].vec30();
        auto z = connector["Z"].vec30();
        auto w = connector["W"].vec31();
        Joint childConnectorJoint(glm::mat4(x, y, z, w));

        if(connector.has("Pinned")) for(auto& pinned : connector["Pinned"]){
            makeModulesRecursively(pinned, childConnectorJoint, module.get());
        }
    }
    return;
}

void VehicleBuilder::setDecals(IModule& module, const Yaml& cfg){
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

void VehicleBuilder::setMarkers(IModule& module, const Yaml& cfg){
    if(not cfg.has("Markers")) return;
    auto& markers = cfg["Markers"];

    for(auto& marker : markers){
        if("Camera"s == marker["Type"].string()){
            auto camera = createModuleFollower(&module, marker["Mode"].string(), marker["W"].vec3());
            camera->offset = marker["Offset"].vec31();
            camera->fov = marker["FOV"].number()*toRad;
            camera->inertia = marker["Inertia"].number();
            camera->recalucuateProjectionMatrix();
            camera->evaluate();

            m_player.eq().cameras.push_back(camera);
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
void VehicleBuilder::setVisual(IModule& module, const Yaml& cfg){
    if(not cfg.has("Models")){
        module.moduleVisualUpdater = std::make_unique<NullModuleVisualUpdater>();
        return;
    }
    auto& models = cfg["Models"].strings();

    m_skinnedMesh->bones.push_back(identityMatrix);
    module.moduleVisualUpdater = std::make_unique<ModuleVisualUpdater>(m_skinnedMesh->bones, m_boneMatrixIndex);
    module.moduleVisualUpdater->setTransform(identityMatrix);

    m_modelLoader->setBoneIndex(m_modelLoader->load(models), m_boneMatrixIndex);

    m_boneMatrixIndex++;
}

// * creates connection between parent and child module, usually this connection is updated by child
// * can have different number of dof
// * lack of limits means that connection is rigid
void VehicleBuilder::setConnection(IModule& module, const Yaml& cfg, Joint& connectorJoint){
    module.joint = connectorJoint;
    module.joint.toBOrigin = cfg["FromParentToOrigin"].vec30();
    if(cfg.has("Limits")){
        module.joint.compileLimits(cfg["Limits"].floats(), cfg["Min"].floats(), cfg["Max"].floats());
    }
    else
        module.joint.setRigidConnection();

    glm::mat4 tr = module.joint.loc();
    module.transform(tr);
}

// * when module has rigidBody created
void VehicleBuilder::setPhysical(IModule& module, const Yaml& cfg){
    if(not cfg.has("Physical")) return;

    auto tr = module.getParentTransform() * module.joint.loc();

    // glm::vec4 cnvPos = glm::vec4(module.joint.toPivot + module.joint.toOrigin);
    auto localTransformation = module.joint.loc();

    if(cfg["Physical"].has("Collision")){
        auto meshes = m_modelLoader->loadCompoundMeshes(cfg["Physical"]["Collision"].strings());

        addToCompound(createCompoundMesh(meshes, (void*)(&module)), localTransformation, (void*)(&module));
        module.moduleCompoundUpdater = std::make_unique<ModuleCompoundUpdater>(m_player.eq().compound, m_compoundIndex++);
    }
    else {
        // TODO: create dummy collision model
    }
}

void VehicleBuilder::setArmor(IModule& module, const Yaml& cfg){}

void VehicleBuilder::addToCompound(btCollisionShape* collShape, const glm::mat4& transform, void* owner){
    btTransform localTrans = convert(transform);
    collShape->setUserPointer(owner);
    m_player.eq().compound->addChildShape(localTrans, collShape);
}


std::shared_ptr<CameraController> VehicleBuilder::createModuleFollower(IModule *module, const std::string& type, glm::vec3 position){
    if(type == "CopyPosition"){
        return m_camFactory.create<ModuleFollower<CopyOnlyPosition>>(module, position);
    }
    else if(type == "CopyTransform"){
        return m_camFactory.create<ModuleFollower<CopyTransform>>(module, position);
    }
    else if(type == "Free"){
        return m_camFactory.create<FreeCamController>();
    }
    return nullptr;
}
