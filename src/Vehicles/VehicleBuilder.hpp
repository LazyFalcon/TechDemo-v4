#pragma once
#include "Yaml.hpp"
#include "ModelLoader.hpp"
#include "ModuleFactory.hpp"

class CameraControllerFactory;
class GraphicComponent;
class IModule;
class Joint;
class PhysicalWorld;
class Player;
class SkinnedMesh;

class VehicleBuilder
{
private:
    std::string m_configName;
    std::unique_ptr<ModelLoader> m_modelLoader;
    PhysicalWorld& m_physicalWorld;
    ModuleFactory m_moduleFactory;
    Player& m_player;
    CameraControllerFactory& m_camFactory;

    std::shared_ptr<SkinnedMesh> m_skinnedMesh;
    uint m_boneMatrixIndex {};
    uint m_compoundIndex {};
    Yaml m_config;

    void openModelFile();
    void makeModulesRecursively(const Yaml& cfg, Joint& connectorJoint, IModule *parentModule);
    void setDecals(IModule& module, const Yaml& cfg);
    void setMarkers(IModule& module, const Yaml& cfg);
    void setVisual(IModule& module, const Yaml& cfg);
    void setConnection(IModule& module, const Yaml& cfg, Joint& connectorJoint);
    void setPhysical(IModule& module, const Yaml& cfg);
    void setArmor(IModule& module, const Yaml& cfg);

    void addToCompound(btCollisionShape* collShape, const glm::mat4& transform, void* owner);
    std::shared_ptr<CameraController> createModuleFollower(IModule *module, const std::string& type, glm::vec3 position);

public:
    VehicleBuilder(const std::string& configName, Player& player, PhysicalWorld& physicalWorld, CameraControllerFactory& camFactory);

    void build();

};
