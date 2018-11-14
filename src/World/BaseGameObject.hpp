#pragma once
#include "BaseGameObjectUtils.hpp"

class BaseGameObject
{
protected:
    ~BaseGameObject(){ // * cannot be used as base pointer for storage, and I don't want to
        if(m_poolObjectIdx){
            m_idProvider.releaseGameObject(m_poolObjectIdx);
        }
    }
    int m_poolObjectIdx {0};

public:
    static GameObjectIdProvider m_idProvider;
    BaseGameObject() : m_poolObjectIdx(m_idProvider.allocateGameObject(this)){}
    BaseGameObject(BaseGameObject&& toMove){
        m_poolObjectIdx = toMove.m_poolObjectIdx;
        toMove.m_poolObjectIdx = 0;

        m_idProvider.get(m_poolObjectIdx).updatePointer(this);
    }

    int indexForBullet(){
        return m_poolObjectIdx;
    }

    GameObjectPtr getHandle(){ // * indirect pointer to this object
        return GameObjectPtr(m_poolObjectIdx);
    }

    // * Interfaces to implement
    uint lastFrame {0};
    virtual void actionWhenVisible() = 0;
    virtual btRigidBody* getCollider() = 0;
};
