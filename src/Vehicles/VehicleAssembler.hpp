#pragma once
#include "ModuleFactory.hpp"
#include "Yaml.hpp"

namespace camera
{
class Factory;
}
class GraphicComponent;
class IModule;
class Joint;
template<typename VertexFormat>
class ModelLoader;
class PhysicalWorld;
class Player;
namespace model
{
class Skinned;
}
struct VertexWithMaterialDataAndBones;

class VehicleAssembler
{
private:
    std::string m_configName;
    std::shared_ptr<ModelLoader<VertexWithMaterialDataAndBones>> m_modelLoader;
    PhysicalWorld& m_physics;
    std::shared_ptr<Vehicle> m_vehicle;
    ModuleFactory m_moduleFactory;
    camera::Factory& m_camFactory;

    std::shared_ptr<model::Skinned> m_skinnedMesh;
    uint m_boneMatrixIndex {};
    uint m_compoundIndex {};
    Yaml m_config;
    bool m_hasAnyPhysicalPart {false};

    void openModelFile();
    void initializeVehicle(const glm::mat4& onPosition);
    void assemblyVehicleModules();
    void finishAssembly(const glm::mat4& onPosition);
    void assemblyModuleAndItsChildren(IModule*, const Yaml&);
    void setServoAndMotionLimits(IModule& module);

    void collectAndInitializeModules();
    void setDecals(IModule& module, const Yaml& cfg);
    void setMarkers(IModule& module, const Yaml& cfg);
    void setVisual(IModule& module, const Yaml& cfg);
    void setPhysical(IModule& module, const Yaml& cfg);
    void setArmor(IModule& module, const Yaml& cfg);
    void attachCameras(IModule& module, const Yaml& names);
    void buildRigidBody(const glm::mat4& onPosition);

    void addToCompound(btCollisionShape* collShape, const glm::mat4& transform, void* owner);

public:
    VehicleAssembler(const std::string& configName, PhysicalWorld& physics, camera::Factory& camFactory);

    std::shared_ptr<Vehicle> build(const glm::mat4& onPosition);
};
