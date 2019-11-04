#pragma once
#include "GeoTimePosition.hpp"

class Yaml;
/*
* Everything that happens in air:
*   Sky
*   Fog
*   Sunlight scattering(probably needs to cooperate with sun)
*/
class Atmosphere
{
public:
    float groundRadius {6.371};
    float skyRadius {6.471};
    // float groundRadius {63.53f};
    // float skyRadius {65.25f};
    float rayleigh {5.5e-3};
    float mie {21e-3};
    float scaleDepth {0.15};
    float cameraHeight {0.0};
    glm::vec4 invWavelength {0.670f, 0.570f, 0.475f, 0};

    GeoTimePosition& geoTimePosition;
    glm::mat4 planetMatrix;

    Atmosphere(const Yaml& sett, GeoTimePosition& geoTimePosition);
    ~Atmosphere();
    void update(glm::vec4 lightDirection);

private:
    bool tweak {false};
};
