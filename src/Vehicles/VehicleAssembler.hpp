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
    std::shared_ptr<Vehicle> m_vehicle;
    ModuleFactory m_moduleFactory;
    CameraControllerFactory& m_camFactory;

    std::shared_ptr<SkinnedMesh> m_skinnedMesh;
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
    std::shared_ptr<CameraController> createModuleFollower(IModule *module, const std::string& type, glm::vec3 position, const glm::mat4& mat);

public:
    VehicleAssembler(const std::string& configName, PhysicalWorld& physics, CameraControllerFactory& camFactory);

    std::shared_ptr<Vehicle> build(const glm::mat4& onPosition);

};
