#pragma once
#include "BaseStructs.hpp"
#include "CloseHitResult.hpp"
#include "BaseGameObject.hpp"

struct ConvexMesh
{
    std::vector<btScalar> data;
    btVector3 position;
};

class PhysicalWorld : private boost::noncopyable
{
public:
    btDbvtBroadphase                       *m_broadphase {nullptr};
    btDefaultCollisionConfiguration        *m_collisionConfiguration {nullptr};
    btCollisionDispatcher                  *m_dispatcher {nullptr};
    btSequentialImpulseConstraintSolver    *m_solver {nullptr};
    btDiscreteDynamicsWorld                *m_dynamicsWorld {nullptr};

    std::vector <btRigidBody*> bodies;
    std::vector <btCollisionShape*> shapes;
    std::vector <btDefaultMotionState*> motionStates;
    std::vector <btTypedConstraint*> constraints;

    PhysicalWorld();
    btRigidBody* createRigidBody(float mass, const btTransform& transform, btCollisionShape* shape, float inertiaScalling=1.f);
    btRigidBody* createRigidBodyWithMasks(float mass, const btTransform& transform, btCollisionShape* shape, short group=2, short collideWith=COL_ALL_BUT_CULLING, float inertiaScalling=1.f);
    void removeBody(btRigidBody *body);
    void update(float step);

    std::pair<SampleResult, btRigidBody*> projectileCollision(btVector3 from, btVector3 to);
    CloseHitResult closesetHit(glm::vec4 from, glm::vec4 to);
};

btCollisionShape* createCompoundShape(std::vector<ConvexMesh> &meshes, void *userPointer=nullptr);
btCollisionShape* createCompoundShape(std::vector<std::pair<btCollisionShape*, btTransform>> &meshes, void *userPointer=nullptr);
