#pragma once

enum class Type
{
    Empty,
    TerrainChunk,
    Actor,
    Enviro,
};

using ObjectID = int;
struct SceneObject
{
public:

    Type type;
    ObjectID ID;
    void *userPointer {nullptr}; // set one of both
    int userID {0};
    static ObjectID nextID(){ // TODO: spójne zapisywanie ID id Id!??!
        return ++ids;
    }
private:
    static ObjectID ids;
};
