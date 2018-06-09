#pragma once
#include "Object.hpp"

struct CloseHitResult
{
    ObjectID objectID;

    bool success;
    glm::vec4 position;
    glm::vec4 normal;
};
