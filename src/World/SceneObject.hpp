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

struct ObjectInterface
{
public:
    virtual ~ObjectInterface() = default;

    virtual void actionVhenVisible() = 0;

    Type type;
    void *userPointer {nullptr};
    int userId {0};
};

struct ObjectAddress
{
    ObjectInterface* o;
};

extern std::list<int> g_freeIndices;
extern std::vector<ObjectAddress> g_sceneObjectAddresses;

inline ObjectInterface& derefIndex(int i){
    return *g_sceneObjectAddresses[i].o;
}

struct ObjectProvider
{
    int index;
    ObjectInterface& operator -> (){
        return derefIndex(index);
    }
};

inline ObjectProvider createProviderFromIndex(int i){
    return {i};
}

// * responsible fro updating object address in array
template<typename T>
struct ObjectWrapper
{
    T t;

    template<typename... Args>
    ObjectWrapper(Args&... args) : t(args...){}
    ~ObjectWrapper() = default;

    ObjectWrapper(const ObjectWrapper& o) : t(o.t){
        arrayIndex = o.arrayIndex;
        updateAddress();
    }
    ObjectWrapper& operator = (const ObjectWrapper& o){
        arrayIndex = o.arrayIndex;
        updateAddress();
    }

    ObjectWrapper(ObjectWrapper&& o) : t(std::move(o.t)){
        arrayIndex = o.arrayIndex;
        updateAddress();
    }
    ObjectWrapper& operator = (ObjectWrapper&& o){
        arrayIndex = o.arrayIndex;
        t = std::move(o.t);
        updateAddress();
    }

    ObjectProvider getProvider(){
        return createProviderFromIndex(arrayIndex);
    }

    void init(){
        arrayIndex = storeAddress();
    }
    void kill(){
        releaseAddress();
    }

    T& operator -> (){
        return t;
    }

private:
    int arrayIndex;

    int storeAddress(){
        if(g_freeIndices.empty()) resizeAddressArray();

        auto i = g_freeIndices.front();
        g_freeIndices.pop_front();

        g_sceneObjectAddresses[i] = {&t, 1};
        return i;
    }
    void updateAddress(){
        g_sceneObjectAddresses[arrayIndex].o = &t;
    }
    void releaseAddress(){
        g_freeIndices.push_back(arrayIndex);
        g_sceneObjectAddresses[arrayIndex] = {};
        arrayIndex = 0;
    }

    void resizeAddressArray(int increase=1000){
        int from = g_sceneObjectAddresses.size();
        g_sceneObjectAddresses.resize(from+increase);

        for(int i = from; i<from+increase; i++)
            g_freeIndices.push_back(i);
    }
};
