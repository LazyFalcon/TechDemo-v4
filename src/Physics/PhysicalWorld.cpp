#include "core.hpp"
#include "PhysicalWorld.hpp"
#include "Logging.hpp"
#include "Utils.hpp"
#include "PerfTimers.hpp"

btCollisionShape* createCompoundMesh(std::vector<ConvexMesh> &meshes, void *userPointer){
    if(meshes.empty()) return nullptr;
    if(meshes.size() == 1){
        auto *collShape = new btConvexHullShape(meshes[0].data.data(), meshes[0].data.size()/3, 3*sizeof(double));
        collShape->setUserPointer(userPointer);
        collShape->setUserIndex(-5);
        return collShape;
    }

    auto *shape = new btCompoundShape();

    for(auto &convex : meshes){
        auto *collShape = new btConvexHullShape(convex.data.data(), convex.data.size()/3, 3*sizeof(double));
        collShape->setUserPointer(userPointer);
        collShape->setUserIndex(-6);

        btTransform localTrans;
        localTrans.setIdentity();
        localTrans.setOrigin(convex.position);
        shape->addChildShape(localTrans, collShape);
    }

    return shape;
}

btCollisionShape* createCompoundMesh(std::vector<std::pair<btCollisionShape*, btTransform>> &meshes, void *userPointer){
    auto *shape = new btCompoundShape();
    shape->setUserPointer(userPointer);
    shape->setUserIndex(-6);

    for(auto &convex : meshes){
        shape->addChildShape(convex.second, convex.first);
    }

    return shape;
}

PhysicalWorld::PhysicalWorld(){

    log("SCALAR SIZE!!!!!!:", sizeof(btScalar));
    // btVector3 worldMin(-2500.0,-2500.0,-500);
    // btVector3 worldMax(2500.0,2500.0,500);
    // broadphase = new btAxisSweep3(worldMin,worldMax);// precyzja pozycjonowania owiata

    m_broadphase = new btDbvtBroadphase();
    m_collisionConfiguration = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
    m_solver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
    m_dynamicsWorld->setGravity(btVector3(0,0,-50));
    // btContactSolverInfo& info = dynamicsWorld->getSolverInfo();
    // info.m_solverMode |= SOLVER_INTERLEAVE_CONTACT_AND_FRICTION_CONSTRAINTS;
    // info.m_numIterations = 100;
    // info.m_minimumSolverBatchSize = 128;
    // info.m_erp = 0.8;
    // info.m_globalCfm = 0.1; FRISCO15
    update(16);
}

btRigidBody* PhysicalWorld::createRigidBody(float mass, const btTransform& transform, btCollisionShape* shape, BodyUser *bodyUser, float inertiaScalling){
    return createRigidBodyWithMasks(mass, transform, shape, bodyUser, 2, COL_ALL_BUT_CULLING, inertiaScalling);
}

// * basically Actor should collide with everything, and everything with actor, and rays and particles
// * masks and groups must match each other to have collision
// * collision flag NO_CONTACT_RESPONSE (t->setCollisionFlags(t->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE)) will disable collision impulses but not calculations
btRigidBody* PhysicalWorld::createRigidBodyWithMasks(float mass, const btTransform& transform, btCollisionShape* shape, BodyUser *bodyUser, short group, short collideWith, float inertiaScalling){
    //@ http://www.continuousphysics.com/Bullet/BulletFull/structbtRigidBody_1_1btRigidBodyConstructionInfo.html
    btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

    btVector3 localInertia(0,0,0);
    if(mass > 0.f) shape->calculateLocalInertia(mass, localInertia);
    localInertia *= inertiaScalling;

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, shape, localInertia);

    cInfo.m_restitution = 0.2;
    cInfo.m_rollingFriction = 0.2;
    cInfo.m_friction = 0.6;
    cInfo.m_linearDamping = 0.3;
    cInfo.m_angularDamping = 0.3;

    btRigidBody* body = new btRigidBody(cInfo);
    // body->setActivationState(DISABLE_DEACTIVATION);
    // TODO: add user counter to body object or make sure that it will be assigned only once
    if(not bodyUser) bodyUser = new BodyUser();
    m_dynamicsWorld->addRigidBody(body, group, collideWith);
    body->setUserPointer(bodyUser);
    bodies.push_back(body);
    shapes.push_back(shape);
    motionStates.push_back(motionState);
    return body;
}

void PhysicalWorld::removeBody(btRigidBody *body){
    m_dynamicsWorld->removeRigidBody(body);
    // delete body->getUserPointer(); // niebezpieczne, co jesli user się powtarza?
    delete body->getMotionState();
    delete body;
}

void PhysicalWorld::update(float step){
    CPU_SCOPE_TIMER("Update world physics");
    m_dynamicsWorld->stepSimulation(step, 10, 1.0/60.0/4.0);

    return;
    int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
    for(u32 i=0; i<numManifolds; i++){
        btPersistentManifold* contactManifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

        btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
        btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

        ((BodyUser*)obA->getUserPointer())->processCollision();
        ((BodyUser*)obB->getUserPointer())->processCollision();

        int numContacts = contactManifold->getNumContacts();
        for(u32 j=0; j<numContacts; j++){
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            if(pt.getDistance()<0.f){
                const btVector3& ptA = pt.getPositionWorldOnA();
                const btVector3& ptB = pt.getPositionWorldOnB();
                const btVector3& normalOnB = pt.m_normalWorldOnB;
            }
        }
    }
}

std::pair<SampleResult, btRigidBody*> PhysicalWorld::projectileCollision(btVector3 from, btVector3 to){
    std::pair<SampleResult, btRigidBody*> result;

    btCollisionWorld::ClosestRayResultCallback closestResults(from,to);
    m_dynamicsWorld->rayTest(from,to,closestResults);
    if(closestResults.hasHit()){
        result.first.succes = closestResults.m_collisionObject->getUserPointer() != nullptr;
        result.first.position = convert(closestResults.m_hitPointWorld, 1);
        result.first.normal = convert(closestResults.m_hitNormalWorld, 0);

        result.second = (btRigidBody*)closestResults.m_collisionObject;
    }
    else {
        result.first.succes = false;
    }

    return result;
}

CloseHitResult PhysicalWorld::closesetHit(glm::vec4 from, glm::vec4 to){
    CloseHitResult result {};

    btCollisionWorld::ClosestRayResultCallback closestResults(convert(from), convert(to));
    m_dynamicsWorld->rayTest(convert(from), convert(to), closestResults);

    if(closestResults.hasHit()){
        result.success = true;
        result.position = convert(closestResults.m_hitPointWorld, 1);
        result.normal = convert(closestResults.m_hitNormalWorld, 0);

        result.objectID = closestResults.m_collisionObject->getUserIndex();
    }
    else {
        result.success = false;
    }

    return result;
}
