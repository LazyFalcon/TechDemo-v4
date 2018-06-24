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

using ObjectID = int;
struct SceneObject
{
public:

    Type type;
    ObjectID ID;
    void *userPointer {nullptr}; // ! of course when user container will be changed this pointer is going to hell
    int userID {0}; // ! this can go to hell too
    static ObjectID nextID(){ // TODO: spójne zapisywanie ID id Id!??!
        return ++ids;
    }
private:
    static ObjectID ids;
};
