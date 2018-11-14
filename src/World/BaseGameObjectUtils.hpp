#pragma once

#define BIT(x) (1<<(x))
enum BulletCollisionMasks
{
    COL_NOTHING = BIT(0),
    COL_DEFAULT = BIT(1),
    COL_ALL_BUT_CULLING = BIT(15)-1,
    COL_FOR_CULLING = BIT(15),
};
#undef BIT
class BaseGameObject;
class btRigidBody;

// * to be used by
// *    GameObjectPtr in most of services to access game object
// *    bullet as one of user pointers
struct BaseGameObjectIntermediateData
{
    int typeUnder {0};
    BaseGameObject* pointerTo;
    void updatePointer(BaseGameObject* ptr){
        pointerTo = ptr;
    }
    BaseGameObject& operator * (){
        return *pointerTo;
    }

    BaseGameObject* operator -> (){
        return pointerTo;
    }
};


// * indirect pointer to object, object is accessed via class above
class GameObjectPtr;
BaseGameObject* deref(int idx);

class GameObjectPtr
{
private:
    int m_poolObjectIdx;
public:
    GameObjectPtr() : m_poolObjectIdx(0) {}
    GameObjectPtr(int p_poolObjectIdx) : m_poolObjectIdx(p_poolObjectIdx) {}
    GameObjectPtr(const GameObjectPtr&) = default;
    GameObjectPtr(GameObjectPtr&&) = default;
    BaseGameObject& operator * (){
        return *(deref(m_poolObjectIdx));
    }
    BaseGameObject* operator -> (){
        return deref(m_poolObjectIdx);
    }
};

class GameObjectIdProvider
{
private:
    std::list<int> m_freeIndices;
    std::vector<BaseGameObjectIntermediateData> m_gameObjectPool;
public:
    GameObjectIdProvider(){
        m_gameObjectPool.push_back({});
        resizeGameObjectPool(5000);
    }
    int allocateGameObject(BaseGameObject* ptr){
        if(m_freeIndices.empty()) resizeGameObjectPool();

        auto i = m_freeIndices.front();
        m_freeIndices.pop_front();

        m_gameObjectPool[i].pointerTo = ptr;
        return i;
    }
    void releaseGameObject(int idx){
        m_gameObjectPool[idx] = {};
        m_freeIndices.push_back(idx);
    }

    void resizeGameObjectPool(int increase=1000){
        int from = m_gameObjectPool.size();
        m_gameObjectPool.resize(from+increase);

        for(int i = from; i<from+increase; i++)
            m_freeIndices.push_back(i);
    }
    BaseGameObjectIntermediateData& get(int idx){
        return m_gameObjectPool[idx];
    }
};
