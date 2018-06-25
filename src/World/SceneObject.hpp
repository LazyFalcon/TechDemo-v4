#pragma once

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

class ObjectHandler;
/*
* When Scene Object can be stored by value, there is a need to provide handler to it to various subsystems(bullet, HUD, scene raycasts, rendering)
* It's nice because ObjectHandler has always the same address when SO can be moved
* So in this cases SO has to update own address in handler
*/
struct SceneObjectInterface
{
public:

    SceneObject();
    virtual ~SceneObject() = default;
    virtual void actionVhenVisible() = 0;

    uint id(){
        return m_id;
    }

    Type type;
    void *userPointer {nullptr};
    int userId {0};

    ObjectStoreProvider handlerToStore;

protected:
    uint m_id;
};

ObjectStoreProvider
{
    int index;
    void update(SceneObjectInterface*);
};

StoreRecord
{
    SceneObjectInterface* o;
    int selfIndex;
};

// * container for pointers to objects, handler index in this container never changes
SceneObjectPointerStore
{
    static std::list<int> freeIndices;
    static std::vector<StoreRecord> container;

    void resizeContainer(int increase=1000){
        size_t from  =g_objectPointerStore.size();
        g_objectPointerStore.resize(from + increase);

        for(int i=from; i<from+increase; i++){
            g_freeStoreIndices.push_back(i);
        }
    }
    ObjectStoreProvider getNext(SceneObjectInterface*); 
};

// * Copyable provider of object, gets it via index from store
struct SceneObjectProvider
{
    SceneObjectProvider(int i) : index(i){}
    const int index;
    SceneObject& operator -> (){
        return SceneObjectPointerStore::get(index);
    }
    
    static SceneObjectProvider get(int i){
        return SceneObjectProvider(i);
    }
    
};

