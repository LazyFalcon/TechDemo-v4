#pragma once
#include "SceneObject.hpp"

struct CloseHitResult
{
    int objectID;

    bool success;
    glm::vec4 position;
    glm::vec4 normal;
};
