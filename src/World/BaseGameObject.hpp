#pragma once

class BaseGameObject;

// * to be used by
// *    GameObjectPtr in most of services to access game object
// *    bullet as one of user pointers
struct BaseGameObjectIntermediateData
{
    BaseGameObject* pointerTo;
    void updatePointer(BaseGameObject* ptr){
        pointerTo = ptr;
    }
    BaseGameObject& operator * (){
        return *m_indirectPointer;
    }

    BaseGameObject* operator -> (){
        return &m_indirectPointer;
    }
};

BaseGameObject* deref(void* ptr){
    return (static_cast<BaseGameObjectIntermediateData*>(ptr)).pointerTo;
}

// * indirect pointer to object, object is accessed via class above
class GameObjectPtr
{
private:
    BaseGameObjectIntermediateData* m_indirectPointer;
public:
    GameObjectPtr() : m_indirectPointer(nullptr) {}
    GameObjectPtr(BaseGameObjectIntermediateData* p_indirectPointer) : m_indirectPointer(p_indirectPointer) {}
    BaseGameObject& operator * (){
        return *(m_indirectPointer.pointerTo);
    }

    BaseGameObject* operator -> (){
        return m_indirectPointer.pointerTo;
    }
};

extern std::array<BaseGameObjectIntermediateData, 5000> g_GameObjectPool;
BaseGameObjectIntermediateData* allocateGameObject();
void releaseGameObject(BaseGameObjectIntermediateData*);


class BaseGameObject
{
private:
    ~BaseGameObject(){ // * cannot be used as base pointer for storage, and I don't want to
        if(m_poolObjectPtr){
            releaseGameObject(m_poolObjectPtr);
        }
    }
    BaseGameObjectIntermediateData* m_poolObjectPtr {nullptr};

public:
    BaseGameObject() = default;
    BaseGameObject(int){
        m_poolObjectPtr = allocateGameObject();
    }
    BaseGameObject(BaseGameObject&& toMove){
        m_poolObjectPtr = toMove.m_poolObjectPtr;
        toMove.m_poolObjectPtr = nullptr;

        m_poolObjectPtr->updatePointer(this);
    }

    BaseGameObjectIntermediateData* pointerForBullet(){
        return m_poolObjectPtr;
    }

    GameObjectPtr getPtr(){ // * indirect pointer to this object
        return GameObjectPtr(m_poolObjectPtr);
    }

    // * Actions to implement
};
