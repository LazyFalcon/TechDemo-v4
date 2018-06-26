#pragma once

struct ObjectInterface;

struct ObjectAddress
{
    ObjectInterface* o;
    int refCount;
};

extern std::list<int> g_freeIndices;
extern std::vector<ObjectAddress> g_sceneObjectAddresses;

inline ObjectInterface* derefIndex(int i){
    return g_sceneObjectAddresses[i].o;
}

struct ObjectProvider
{
    int index;
    ObjectInterface* operator -> (){
        return derefIndex(index);
    }
};

inline ObjectProvider createProviderFromIndex(int i){
    return {i};
}

// * responsible fro updating object address in array
// TODO: maybe add this ref counter?
template<typename T>
struct ObjectWrapper
{
    bool isVisible {false};
    T t;

    ~ObjectWrapper(){
        releaseAddress();
    }

    template<typename... Args>
    ObjectWrapper(Args&... args) : t(args...), m_arrayIndex(0){
        init();
    }
    ObjectWrapper(T&& o) : t(std::move(t)), m_arrayIndex(0){
        init();
        updateAddress();
    }

    ObjectWrapper(const ObjectWrapper& o) : t(o.t){
        m_arrayIndex = o.m_arrayIndex;
        updateAddress();
    }

    ObjectWrapper& operator = (const ObjectWrapper& o){
        m_arrayIndex = o.m_arrayIndex;
        updateAddress();
    }

    ObjectWrapper(ObjectWrapper&& o) : t(std::move(o.t)){
        m_arrayIndex = o.m_arrayIndex;
        updateAddress();
    }


    ObjectWrapper& operator = (ObjectWrapper&& o){
        m_arrayIndex = o.m_arrayIndex;
        t = std::move(o.t);
        updateAddress();
    }

    ObjectProvider getProvider(){
        return createProviderFromIndex(m_arrayIndex);
    }

    // * could be called automaticaly when wrapper is initialized by T
    void init(){
        if(m_arrayIndex) releaseAddress();
        m_arrayIndex = storeAddress();
    }

    // * needs to be called to free space in array
    void kill(){
        releaseAddress();
    }

    T& operator * (){
        return t;
    }

    T* operator -> (){
        return &t;
    }

    int id(){
        return m_arrayIndex;
    }

private:
    int m_arrayIndex {0};

    int storeAddress(){
        if(g_freeIndices.empty()) resizeAddressArray();

        auto i = g_freeIndices.front();
        g_freeIndices.pop_front();

        g_sceneObjectAddresses[i] = {&t, 1};
        return i;
    }
    void updateAddress(){
        ++g_sceneObjectAddresses[m_arrayIndex].refCount;
        g_sceneObjectAddresses[m_arrayIndex].o = &t;
    }
    void releaseAddress(){
        --g_sceneObjectAddresses[m_arrayIndex].refCount;
        if(not m_arrayIndex or g_sceneObjectAddresses[m_arrayIndex].refCount) return;
        g_freeIndices.push_back(m_arrayIndex);
        g_sceneObjectAddresses[m_arrayIndex] = {};
        m_arrayIndex = 0;
    }

    void resizeAddressArray(int increase=1000){
        int from = g_sceneObjectAddresses.size();
        g_sceneObjectAddresses.resize(from+increase);

        for(int i = from; i<from+increase; i++)
            g_freeIndices.push_back(i);
    }
};
