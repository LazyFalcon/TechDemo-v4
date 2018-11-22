#pragma once
#include "Yaml.hpp"
#include "ModuleFactory.hpp"

class CameraControllerFactory;
class GraphicComponent;
class IModule;
class Joint;
template<typename VertexFormat>
class ModelLoader;
class PhysicalWorld;
class Player;
class SkinnedMesh;
struct VertexWithMaterialDataAndBones;

class VehicleAssembler
{
private:
    std::string m_configName;
    std::shared_ptr<ModelLoader<VertexWithMaterialDataAndBones>> m_modelLoader;
    PhysicalWorld& m_physics;
    std::shared_ptr<VehicleEquipment> m_vehicleEq;
    ModuleFactory m_moduleFactory;
    Player& m_player;
    CameraControllerFactory& m_camFactory;

    std::shared_ptr<SkinnedMesh> m_skinnedMesh;
    uint m_boneMatrixIndex {};
    uint m_compoundIndex {};
    Yaml m_config;

    void openModelFile();
    void makeModulesRecursively(const Yaml& cfg, const Yaml& connectorProp, IModule *parentModule);
    void setDecals(IModule& module, const Yaml& cfg);
    void setMarkers(IModule& module, const Yaml& cfg);
    void setVisual(IModule& module, const Yaml& cfg);
    void setConnection(IModule& module, const Yaml& cfg);
    void setPhysical(IModule& module, const Yaml& cfg);
    void setArmor(IModule& module, const Yaml& cfg);
    void buildRigidBody(glm::vec4 onPosition);

    void addToCompound(btCollisionShape* collShape, const glm::mat4& transform, void* owner);
    std::shared_ptr<CameraController> createModuleFollower(IModule *module, const std::string& type, glm::vec3 position);

public:
    VehicleAssembler(const std::string& configName, Player& player, PhysicalWorld& physics, CameraControllerFactory& camFactory);

    void build(glm::vec4 onPosition);

};
