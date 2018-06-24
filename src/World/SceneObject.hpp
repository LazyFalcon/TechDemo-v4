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

    std::shared_ptr<ObjectHandler_> handlerToObject;

protected:
    uint m_id;
};

// struct DummySceneObjectInterface : public SceneObjectInterface
// {};


/*
* It's only job is to provide current address of object that is hold elsewhere
* Using this via shared ptr
*/
struct SceneObjectProvider_
{
    SceneObjectInterface* handlee;
};

using ObjectProvider = std::shared_ptr<SceneObjectProvider_>;
