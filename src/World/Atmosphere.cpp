#include "Includes.hpp"
#include "Atmosphere.hpp"

#include "Yaml.hpp"
#include <glm/gtx/rotate_vector.hpp>

Atmosphere::Atmosphere(const Yaml &sett, GeoTimePosition &geoTimePosition) : geoTimePosition(geoTimePosition){
    groundRadius = sett["PlanetRaduis"].number();
    skyRadius = sett["AtmosphereRadius"].number();
    rayleigh = sett["Rayleigh"].number();
    mie = sett["Mie"].number();
    scaleDepth = sett["ScaleDepth"].number();
    invWavelength = sett["InvWaveLength"].vec4();
    tweak = sett["Tweak"].boolean();
}
Atmosphere::~Atmosphere(){
}

void Atmosphere::update(glm::vec4 lightDirection){
    float angleZRotation = geoTimePosition.m_date.date().day_of_year() / 365.25 * pi2;
    glm::mat4 globalZRotation = glm::rotate(angleZRotation, Z3);

    glm::mat4 localPlanetRotation = glm::orientation(lightDirection.xyz(), -Z3);

    // planetMatrix = globalZRotation * localPlanetRotation;
    // planetMatrix = localPlanetRotation;
    auto time = geoTimePosition.m_date.time_of_day();
    // float rotPart = (time.hours()/24.f + time.minutes()/60.f + time.seconds()/3600.f)/(24.f*60.f*60.f) * pi2;
    float rotPart = (time.hours()/24.f + time.minutes()/60.f/24.f + time.seconds()/60.f/60.f/24.f) * pi2;
    planetMatrix = glm::rotate(pi - geoTimePosition.latitude, X3) * glm::rotate(rotPart, -Z3) ;
    // planetMatrix = glm::rotate(rotPart, -Z3) ;
}
