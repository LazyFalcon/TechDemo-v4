#pragma once
#include "GeoTimePosition.hpp"

class Atmosphere;
class Yaml;

/**
 *  Aproximated sun position calculation
 *  https://nicoschertler.wordpress.com/2013/04/03/simulating-a-days-sky/
 *  http://www.boost.org/doc/libs/1_55_0/doc/html/date_time/date_time_io.html#date_time.date_facet
 *
 */
class Sun
{
public:
    Sun(const Yaml &sett, GeoTimePosition &geoTimePosition);
    ~Sun();
    void update(Atmosphere &atm);
    glm::vec4 getVector(){
        return lightDirectionVector;
    }
    glm::vec4 getLightVector();
    glm::vec3 getColor();

    glm::vec4 lightDirectionVector {}; // to or from sun? **to**
    glm::mat4 lightTransform {};
    glm::vec3 lightColor {};
    float size;
    float power;
private:
    glm::vec4 polarToVector();
    void calcSunPosition();
    glm::vec3 calcSunColor(Atmosphere &atmosphere);
    GeoTimePosition &geoTimePosition;
    // sun position, on sky, polar
    float azimuth {pi}; // horizontal, radians, north has 0, south pi
    float elevation {pi}; // vertical, radians
    float zenithAngle {0}; // between sunDir and Z vector
    float height {0};

    bool tweak;

    float radialSize {0.15};
    HexColor color;
};
