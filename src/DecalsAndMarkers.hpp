#pragma once

struct Decal
{
    glm::mat4 transform;
    float layer;

    void update(const glm::mat4 &tr){
        decalList.push_back({tr*transform, layer});

        // if(/**Debug*/true) addDebugCube().mat(decalList.back().transform);
    }
    static std::vector<Decal> decalList;
};

struct Marker
{
    glm::vec4 direction;
    glm::vec4 up;
    glm::vec4 position;
    glm::vec4 baseDirection;
    glm::vec4 baseUp;
    glm::vec4 baseoPsition;

    void update(const glm::mat4 &tr){
        direction = tr * baseDirection;
        up = tr * baseUp;
        position = tr * baseoPsition;
    }
};
