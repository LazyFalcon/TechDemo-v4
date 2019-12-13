#pragma once
#include "object-id.hpp"

#define BIT(x) (1 << (x))
enum BulletCollisionMasks
{
    COL_NOTHING = BIT(0),
    COL_DEFAULT = BIT(1),
    COL_ALL_BUT_CULLING = BIT(15) - 1,
    COL_FOR_CULLING = BIT(15),
};
#undef BIT

enum class GameType
{
    Actor,
    LightSource,
    Camera,
    Dummy,
    Tree,
    Cluster,
    Cell
};
class BaseOfGameObject;
class ObjectHandle
{
private:
    int m_globalIndex;

public:
    ObjectHandle() = default;
    ObjectHandle(int i) : m_globalIndex(i) {}

    BaseOfGameObject& operator*() {
        return *(utils::deref(m_globalIndex));
    }
    BaseOfGameObject* operator->() {
        return utils::deref(m_globalIndex);
    }
};

class BaseOfGameObject : public ObjectId
{
public:
    BaseOfGameObject(GameType type) : type(type) {}
    std::string name;
    GameType type;

    // * Interfaces to implement
    uint lastFrame {0};
    // todo: make one generic function that will perform basic checks(i.e. frame number) and will call this one
    virtual void actionWhenVisible() = 0;
    virtual void addToShadowmap() = 0;
    virtual btRigidBody* getCollider() = 0;

    ObjectHandle createHandle() {
        return ObjectHandle(ObjectId::getId());
    }
};
