#pragma once
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "Object.hpp"
#include <vector>


class DbvtBroadphaseFrustumCulling : public btDbvt::ICollide
{
public:
    std::vector<ObjectID> objectsInsideFrustum;
    std::vector<int> cellsInsideFrustum;
    short int collisionFilterMask;

    DbvtBroadphaseFrustumCulling()
    : collisionFilterMask(btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger)
    { }
    bool Descent(const btDbvtNode* node){
        return true;
    }
    void Process(const btDbvtNode* node, btScalar depth){Process(node);}
    void Process(const btDbvtNode* leaf){
        btBroadphaseProxy *proxy = static_cast<btBroadphaseProxy*>(leaf->data);
        btCollisionObject *co = static_cast<btCollisionObject*>(proxy->m_clientObject);

        if((proxy->m_collisionFilterGroup & collisionFilterMask) != 0){
            if(co->getUserIndex()>0) objectsInsideFrustum.push_back(co->getUserIndex());
        }
    }
};
