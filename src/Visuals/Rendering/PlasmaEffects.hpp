#pragma once
#include "Color.hpp"
#include "Events.hpp"
#include "visuals-prepared-scene.hpp"


class PlasmaFlashEvent : public Event
{
public:
    glm::vec4 position;
    float sizeA {0.05f};
    float sizeB {1.f};
    float lifetime {100};

    struct Sparkle
    {};
    std::vector<Sparkle> sparkles;

    bool handle(App& app) { /// dodać dt?
        lifetime -= 16.f;
        if(lifetime < 0)
            return true;

        // if(lifetime > 170){
        sizeA *= 2;
        // }
        // else {
        //     sizeA *= 0.1;
        // }
        // visuals::preparedScene.insert(*this);
        visuals::preparedScene.insert(PointLightSource {position, Color(238, 239, 192, 255), 40.f, 0.5f});
        return false;
    }
};
