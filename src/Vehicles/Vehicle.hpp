#pragma once
#include "BulletDynamics\Dynamics\btActionInterface.h"
#include "FireControlSystem.hpp"
#include "GraphicComponent.hpp"
#include "MountedWeapon.hpp"
#include "VehicleControlSystem.hpp"
#include "camera-collection-of.hpp"
#include "camera-controller.hpp"
/**
    Stores informations about vehicle(universal for each type, vehicle is build from modules), can be used in HUD
    Where goes hitpoints, or vehicle vitality
*/
class Actor;
class IModule;
class PhysicalWorld;
class PowerShield;
class Radar;
class Suspension;

struct VehicleControl
{
    std::optional<glm::vec4> virtualPosition;
    std::optional<glm::vec4> virtualDirection;
    glm::vec4 controlOnAxes {};
    glm::vec4 aimingAt {};
};

struct VehicleSharedData
{
    std::optional<glm::vec4> targetPoint;
    std::optional<int> targetEnemy;
    std::vector<int> knownEnemies;
};

class Vehicle : public btActionInterface
{
public:
    Vehicle(PhysicalWorld& physics);
    ~Vehicle();
    void init();
    void updateModules(float dt);
    void updateMarkers();
    void debugDraw(btIDebugDraw* debugDrawer) override {}
    void updateAction(btCollisionWorld* collisionWorld, btScalar dt) override;

    PhysicalWorld& physics;
    VehicleControl control {};
    GraphicComponent graphics; // ? make vehicle more avare of its parts? or just make it list of models
    // * Trzeba gdzieś tu wrzucić dane które będą służyć do komunikacji z modułami, w obie strony
    VehicleSharedData shared;

    btRigidBody* rgBody {nullptr};
    btCompoundShape* compound {nullptr};
    Actor* actor {nullptr};

    float vitality; /// kind of hitpoints, derived from vitality of modules
    btTransform rotation;
    btVector3 forward;
    btVector3 right;
    glm::vec4 getPosition() {
        return glTrans[3];
    }

    void lockCannonsInDefaultPosition() {
        lockCannons = !lockCannons;
    }
    bool lockCannons {false};
    void drawBBOXesOfChildren();

    btTransform btTrans;
    glm::mat4 invTrans;
    glm::mat4 glTrans;
    // std::vector<btTransform> transforms;
    std::vector<std::shared_ptr<IModule>> modules; /// hierarchical for updateMarkers
    // std::vector<std::shared_ptr<IModule>> modulesToUpdateInsidePhysicsStep;
    camera::Collection cameras;
    std::vector<MountedWeapon> mountedWeapons;
    // std::unique_ptr<PowerShield> powerShield;
    // std::unique_ptr<Radar> radar;
    std::unique_ptr<FireControlSystem> fireControlUnit;
    std::unique_ptr<VehicleControlSystem> vehicleControlUnit;
    std::map<std::string, int> resources; // todo: expand later
};
