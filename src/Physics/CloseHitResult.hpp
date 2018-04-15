#pragma once
#include "Includes.hpp"
#include "Object.hpp"

struct CloseHitResult
{
    ObjectID objectID;

    bool success;
    glm::vec4 position;
    glm::vec4 normal;
};
