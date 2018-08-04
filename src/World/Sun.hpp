#pragma once
#include "GeoTimePosition.hpp"

class Atmosphere;
class Yaml;

/*
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

    enum {Alone, WithAmbient, WithIR} shader {Sun::WithAmbient};

    glm::mat4 transform;
    glm::vec4 direction;
    glm::vec4 color;
    float power;
    float size;

private:
    GeoTimePosition &m_geoTimePosition;

    glm::vec4 polarToVector();
    void calcSunPosition();
    glm::vec4 calcSunColor(Atmosphere &atmosphere);

    // * sun position, on sky, polar calculated from geoTime
    float azimuth {pi}; // horizontal, radians, north has 0, south pi
    float elevation {pi}; // vertical, radians
    float zenithAngle {0}; // between sunDir and Z vector
    float height {0};

    bool tweak;

    float radialSize {0.15};
};
