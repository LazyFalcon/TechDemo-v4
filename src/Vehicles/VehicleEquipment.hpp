#pragma once
#include "CameraController.hpp"
#include "BulletDynamics\Dynamics\btActionInterface.h"
#include "GraphicComponent.hpp"
/**
    Stores informations about vehicle(universal for each type, vehicle is build from modules), can be used in HUD
    Where goes hitpoints, or vehicle vitality
*/
class Actor;
class DriveSystem;
class IModule;
class PhysicalWorld;
class PowerShield;
class Radar;
class SKO;
class Suspension;

struct VehicleControl
{
    std::optional<glm::vec4> targetPoint;
};

class VehicleEquipment : public btActionInterface
{
public:
    VehicleEquipment(PhysicalWorld& physics) : physics(physics){}
    void init();
    void updateModules(float dt);
    void updateMarkers();
    void debugDraw(btIDebugDraw *debugDrawer) override {}
    void updateAction(btCollisionWorld *collisionWorld, btScalar dt) override ;

    PhysicalWorld& physics;
    VehicleControl control;
    GraphicComponent graphics;

    btRigidBody *rgBody {nullptr};
    btCompoundShape *compound {nullptr};
    Actor *actor {nullptr};

    float vitality; /// kind of hitpoints, derived from vitality of modules
    btTransform rotation;
    btVector3 forward;
    btVector3 right;
    void setTargetPoint(glm::vec4 target);
    // glm::vec3 getPosition(){
    //     return (glTrans[3]).xyz();
    // }
    glm::vec4 getPosition(){
        return glTrans[3];
    }

    void lockCannonsInDefaultPosition(){
        lockCannons = !lockCannons;
    }
    bool lockCannons {false};
    void drawBBOXesOfChildren();

    btTransform btTrans;
    glm::mat4 invTrans;
    glm::mat4 glTrans;
    // std::vector<btTransform> transforms;
    std::vector<std::shared_ptr<IModule>> modules; /// hierarchical for updateMarkers
    std::vector<std::shared_ptr<IModule>> modulesToUpdateInsidePhysicsStep;
    std::vector<std::shared_ptr<CameraController>> cameras;
    std::shared_ptr<PowerShield> powerShield;
    std::shared_ptr<DriveSystem> driveSystem; /// instead of while class Vehicle, this unit is responsible for movement, and Actor for knowing which type it is
    std::shared_ptr<Radar> radar;
    std::shared_ptr<SKO> sko;
};
