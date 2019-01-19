#pragma once

enum CommandType
{
    MoveTo, Attack
};

struct AiCommand
{
    CommandType type;
    bool suspendOther;
    bool queue;
    void* payload;
    template<typename T>
    T& get(){
        return *reinterpret_cast<T*>(payload);
    }
};

struct MoveCommand
{
    glm::vec4 position;
    glm::vec4 direction;
};
