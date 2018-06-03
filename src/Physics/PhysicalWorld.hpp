#pragma once
#include "common.hpp"
#include "BaseStructs.hpp"
#include "BodyUser.hpp"
#include "Object.hpp"
#include "CloseHitResult.hpp"

struct ConvexMesh
{
    std::vector<double> data;
    btVector3 position;
};

class PhysicalWorld
{
public:
    btBroadphaseInterface                  *broadphase {nullptr};
    btDefaultCollisionConfiguration        *collisionConfiguration {nullptr};
    btCollisionDispatcher                  *dispatcher {nullptr};
    btSequentialImpulseConstraintSolver    *solver {nullptr};
    btDiscreteDynamicsWorld                *dynamicsWorld {nullptr};
    btSequentialImpulseConstraintSolver    *constraintSolver {nullptr};

    std::vector <btRigidBody*> bodies;
    std::vector <btCollisionShape*> shapes;
    std::vector <btDefaultMotionState*> motionStates;
    std::vector <btTypedConstraint*> constraints;

    void init();
    btRigidBody* createRigidBody(float mass, const btTransform& transform, btCollisionShape* shape, BodyUser *go=nullptr, float inertiaScalling=1.f);
    btRigidBody* createRigidBodyWithMasks(float mass, const btTransform& transform, btCollisionShape* shape, BodyUser *bodyUser=nullptr, short group=2, short masks=-1, float inertiaScalling=1.f);
    void removeBody(btRigidBody *body);
    void update(float step);

    std::pair<SampleResult, btRigidBody*> projectileCollision(btVector3 from, btVector3 to);
    CloseHitResult closesetHit(glm::vec4 from, glm::vec4 to);
};

btCollisionShape* createCompoundMesh(std::vector<ConvexMesh> &meshes, void *userPointer=nullptr);
btCollisionShape* createCompoundMesh(std::vector<std::pair<btCollisionShape*, btTransform>> &meshes, void *userPointer=nullptr);
