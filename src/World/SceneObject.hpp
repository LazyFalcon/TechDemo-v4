#pragma once
#include "SceneObjectWrapper.hpp"

enum class Type
{
    Empty,
    TerrainChunk,
    Actor,
    Enviro,
};

#define BIT(x) (1<<(x))
enum BulletCollisionMasks
{
    COL_NOTHING = BIT(0),
    COL_DEFAULT = BIT(1),
    COL_ALL_BUT_CULLING = BIT(15)-1,
    COL_FOR_CULLING = BIT(15),
};
#undef BIT

struct ObjectInterface
{
public:
    virtual ~ObjectInterface() = default;

    virtual void actionWhenVisible() = 0;
    virtual btRigidBody* getCollider() = 0;

    Type type;
    uint lastFrame {0};
};
