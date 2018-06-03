#pragma once

class Joint
{
public:
    glm::mat4 getDefaultTransform(){
        return glm::mat4();
    }


    glm::vec4 fromAOrigin;
    glm::vec4 toBOrigin;

    float min, max;
    int axis;

    float value;
};
