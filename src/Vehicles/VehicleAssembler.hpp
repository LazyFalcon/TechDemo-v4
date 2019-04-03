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
    struct ToBuildModuleLater
    {
        std::shared_ptr<IModule> module;
        glm::vec4 fromJointToOrigin;
        int configId;
    };
    std::string m_configName;
    std::shared_ptr<ModelLoader<VertexWithMaterialDataAndBones>> m_modelLoader;
    PhysicalWorld& m_physics;
    std::shared_ptr<VehicleEquipment> m_vehicleEq;
    ModuleFactory m_moduleFactory;
    CameraControllerFactory& m_camFactory;

    std::shared_ptr<SkinnedMesh> m_skinnedMesh;
    uint m_boneMatrixIndex {};
    uint m_compoundIndex {};
    Yaml m_config;
    std::map<std::string, ToBuildModuleLater> modules;

    void openModelFile();
    void collectAndInitializeModules();
    void connectModules(ToBuildModuleLater& module, IModule* parent, const Yaml* connectionProps);
    void setDecals(IModule& module, const Yaml& cfg);
    void setMarkers(IModule& module, const Yaml& cfg);
    void setVisual(IModule& module, const Yaml& cfg);
    void setConnection(IModule& module, const Yaml& cfg);
    void setPhysical(IModule& module, const Yaml& cfg);
    void setArmor(IModule& module, const Yaml& cfg);
    void buildRigidBody(const glm::mat4& onPosition);

    void addToCompound(btCollisionShape* collShape, const glm::mat4& transform, void* owner);
    std::shared_ptr<CameraController> createModuleFollower(IModule *module, const std::string& type, glm::vec3 position, const glm::mat4& mat);

public:
    VehicleAssembler(const std::string& configName, PhysicalWorld& physics, CameraControllerFactory& camFactory);

    std::shared_ptr<VehicleEquipment> build(const glm::mat4& onPosition);

};
