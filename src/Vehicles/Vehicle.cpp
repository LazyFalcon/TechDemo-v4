#include "core.hpp"
#include "Vehicle.hpp"
#include "IModule.hpp"

Vehicle::Vehicle(PhysicalWorld& physics) : physics(physics){}
Vehicle::~Vehicle() = default;

void Vehicle::drawBBOXesOfChildren(){
    return;
    for(i32 i=compound->getNumChildShapes()-1; i>=0; --i){
        btCollisionShape *shape = compound->getChildShape(i);
        btVector3 min, max;
        shape->getAabb(btIdentity, min, max);

        // addDebugCube().fromBtAabb(min, max, btTrans*compound->getChildTransform(i));
    }
}
void Vehicle::updateModules(float dt){
    for(auto &module : modules){
        module->update(dt);
    }
}
void Vehicle::updateMarkers(){
}

void Vehicle::updateAction(btCollisionWorld *collisionWorld, btScalar dt){
    vehicleControlUnit->updateInsidePhysicsStep(dt);
    // todo: collect collisions for modules
}
