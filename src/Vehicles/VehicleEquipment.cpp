#include "core.hpp"
#include "VehicleEquipment.hpp"
#include "IModule.hpp"

void VehicleEquipment::drawBBOXesOfChildren(){
    return;
    for(i32 i=compound->getNumChildShapes()-1; i>=0; --i){
        btCollisionShape *shape = compound->getChildShape(i);
        btVector3 min, max;
        shape->getAabb(btIdentity, min, max);

        // addDebugCube().fromBtAabb(min, max, btTrans*compound->getChildTransform(i));
    }
}
void VehicleEquipment::updateModules(float dt){
    for(auto &module : modules){
        module->update(dt);
        module->updateCommon(dt);
    }
}
void VehicleEquipment::updateMarkers(){
    for(auto &module : modules){
        module->updateMarkers();
        module->updateDecals();
    }
}

void VehicleEquipment::setTargetPoint(glm::vec4 target){
    // sko->updateTarget(target);
}
