#pragma once

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

namespace utils
{
void releaseIndex(int);
int acquireIndex();
void updatePointer(int, BaseOfGameObject*);
BaseOfGameObject* deref(int);
BaseOfGameObject* deref(const btRigidBody*);
void setRef(int, btRigidBody*);
}

class ObjectHandle
{
private:
    int m_globalIndex {0};

public:
    ObjectHandle() = default;
    ObjectHandle(int i) : m_globalIndex(i) {}
    ObjectHandle(const ObjectHandle& handle) {
        m_globalIndex = handle.m_globalIndex;
    }
    ObjectHandle& operator=(const ObjectHandle& handle) {
        m_globalIndex = handle.m_globalIndex;
        return *this;
    }
    ObjectHandle& operator=(ObjectHandle&& parent) = delete;
    ObjectHandle(ObjectHandle&& parent) = delete;

    BaseOfGameObject& operator*() {
        return *(utils::deref(m_globalIndex));
    }
    BaseOfGameObject* operator->() {
        return utils::deref(m_globalIndex);
    }
};

class BaseOfGameObject
{
protected:
    int m_globalIndex {0};

public:
    BaseOfGameObject(GameType type) : m_globalIndex(utils::acquireIndex()), type(type) {
        utils::updatePointer(m_globalIndex, this);
    }
    virtual ~BaseOfGameObject() = default;
    BaseOfGameObject(const BaseOfGameObject&) = delete;
    BaseOfGameObject& operator=(const BaseOfGameObject&) = delete;
    BaseOfGameObject& operator=(BaseOfGameObject&& parent) {
        m_globalIndex = parent.m_globalIndex;
        utils::updatePointer(m_globalIndex, this);
        return *this;
    }
    BaseOfGameObject(BaseOfGameObject&& parent) {
        m_globalIndex = parent.m_globalIndex;
        utils::updatePointer(m_globalIndex, this);
    }
    std::string name;
    GameType type;

    int getId() const {
        return m_globalIndex;
    }

    void setRef(btRigidBody* b) {
        utils::setRef(m_globalIndex, b);
    }

    // * Interfaces to implement
    uint lastFrame {0};
    // todo: make one generic function that will perform basic checks(i.e. frame number) and will call this one
    virtual void actionWhenVisible() = 0;
    virtual void addToShadowmap() = 0;
    virtual btRigidBody* getCollider() = 0;

    ObjectHandle createHandle() {
        return ObjectHandle(getId());
    }
};
