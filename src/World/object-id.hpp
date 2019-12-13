#pragma once
class btRigidBody;
class ObjectId;
class BaseOfGameObject;
namespace utils
{
void releaseIndex(int);
int acquireIndex();
void updatePointer(int, ObjectId*);
BaseOfGameObject* deref(int);
BaseOfGameObject* deref(const btRigidBody*);
void setRef(int, btRigidBody*);
}

class ObjectId // must be first in list of super classes
{
protected:
    ~ObjectId() {
        utils::releaseIndex(m_globalIndex);
    }
    int m_globalIndex {0};

public:
    ObjectId() : m_globalIndex(utils::acquireIndex()) {
        utils::updatePointer(m_globalIndex, this);
    }
    ObjectId(const ObjectId&) = delete;
    ObjectId& operator=(const ObjectId&) = delete;
    ObjectId& operator=(ObjectId&& parent) {
        m_globalIndex = parent.m_globalIndex;
        utils::updatePointer(m_globalIndex, this);
        return *this;
    }
    ObjectId(ObjectId&& parent) {
        m_globalIndex = parent.m_globalIndex;
        utils::updatePointer(m_globalIndex, this);
    }

    int getId() const {
        return m_globalIndex;
    }

    void setRef(btRigidBody* b) {
        utils::setRef(m_globalIndex, b);
    }
};
