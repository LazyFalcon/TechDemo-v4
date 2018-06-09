#include "core.hpp"
#include "PMK.hpp"
#include "Sun.hpp"
#include "Atmosphere.hpp"

#include "SunPos.hpp"
#include "Yaml.hpp"
#include "Colors.hpp"

Sun::Sun(const Yaml &sett, GeoTimePosition &geoTimePosition) : geoTimePosition(geoTimePosition), lightDirectionVector(glm::normalize(glm::vec4(20,36,-10,0))){
    size = sett["Size"].number();
    power = sett["Power"].number();
    tweak = sett["Tweak"].boolean();

    calcSunPosition();
    pmk::sunSpaceViewBox = [this](glm::vec4 &bbMin, glm::vec4 &bbMax, glm::vec4 position){
        position = lightTransform*position;
        bbMin = pmk::min(position, bbMin);
        bbMax = pmk::max(position, bbMax);
    };
    pmk::updateSunTransform = [this](glm::vec4 p, glm::vec4 cameraView){
        lightTransform = glm::lookAt(p.xyz()-getLightVector().xyz(), p.xyz(), Z3);
    };
}
Sun::~Sun(){
}

glm::vec4 Sun::polarToVector(){
    float x = cos(elevation)*cos(azimuth);
    float y = cos(elevation)*sin(azimuth);
    float z = sin(elevation);

    glm::vec4 vec(0,0,1,0);
    vec = glm::rotateY(vec, zenithAngle);
    vec = glm::rotateZ(vec, azimuth);
    return -glm::normalize(vec);

    return glm::normalize(glm::vec4(glm::euclidean(glm::vec2(elevation, azimuth)),0));
}

void Sun::calcSunPosition(){
    auto coords = sunpos::calc({// TODO: pass julian days and hour of the day, use u32
                                (int)geoTimePosition.date().year(),
                                (int)geoTimePosition.date().month(),
                                (int)geoTimePosition.date().day(),
                                (double)geoTimePosition.time().hours(),
                                (double)geoTimePosition.time().minutes(),
                                (double)geoTimePosition.time().seconds()
                            }, {
                                geoTimePosition.longitude,
                                geoTimePosition.latitude
                            });

    zenithAngle = coords.zenithAngle*toRad;
    elevation = pi*0.5 - zenithAngle;
    azimuth = coords.azimuth*toRad;
}

// sun light intensity: https://pl.wikipedia.org/wiki/Promieniowanie_s%C5%82oneczne
glm::vec3 Sun::calcSunColor(Atmosphere &atm){

    glm::dvec4 invWavelength = 1.f/glm::pow4(atm.invWavelength);
    auto uScale = 1/(atm.skyRadius - atm.groundRadius);
    float uScaleDepth = atm.scaleDepth;
    auto scale = [uScaleDepth](float fCos){
        float x = 1.0 - fCos;
        return uScaleDepth * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
    };

    glm::vec3 lightDir = glm::normalize(-lightDirectionVector.xyz());
    glm::vec3 cameraPosition = glm::vec3(0, 0, atm.groundRadius + 0.00f/* + uCameraHeight/50000*/);
    glm::vec3 cameraRay = -lightDir;

    double angle = glm::dot(cameraRay, glm::vec3(0,0,-1));
    double B =  - 2 * cameraPosition.z * glm::dot(cameraRay, glm::vec3(0,0,-1));
    double C = powf(cameraPosition.z, 2.f) - powf(atm.skyRadius, 2.f);
    double delta = B*B - 4*C;
    float farDistance = (B - sqrt(delta)) / C * 0.5f;
    glm::vec3 farPosition = cameraPosition + cameraRay*farDistance;

    glm::vec3 start = cameraPosition;
    double height = glm::length(start);
    double scaleOverScaleDepth = uScale / uScaleDepth;
    double depth = exp(scaleOverScaleDepth * ( atm.groundRadius - height));
    double startAngle = glm::dot(cameraRay, start) / height;
    double startOffset = depth*scale(startAngle);

    glm::dvec3 color = glm::vec3(0);
    float sampleLength = farDistance * 0.25f;
    double scaledLength = sampleLength * uScale;
    glm::vec3 sampleRay = cameraRay * sampleLength;
    glm::vec3 samplePoint = start + sampleRay * 0.5f;

    for(int i=0; i<4; i++){
        height = length(samplePoint);
        depth = exp(scaleOverScaleDepth * (atm.groundRadius - height));
        double lightAngle = glm::dot(lightDir, samplePoint) / height;
        double cameraAngle = glm::dot(cameraRay, samplePoint) / height;
        double scatter = (startOffset + depth*(scale(lightAngle) - scale(cameraAngle)));
        glm::dvec3 attenuate = exp(-scatter * (invWavelength.xyz() * double(atm.rayleigh*4.0*pi) + double(atm.mie*4.0*pi)) );
        color += attenuate * (depth * scaledLength);
        samplePoint += sampleRay;
    }
    glm::dvec3 rayleighColor(color * invWavelength.xyz() * double(atm.mie*25.0));
    glm::dvec3 mieColor(color * double(atm.rayleigh*25.0));

    double lightAngle = 0.999;
    double lightAngle2 = lightAngle * lightAngle;

    double g = -0.9995;
    double g2 = g*g;
    double miePhase = 1.5*((1.0-g2)/(2.0+g2))*(1.0+lightAngle2)/pow(1.0+g2 - 2.0*g*lightAngle, 1.5);
    double rayleighPhase = 0.75 * (2.0 + 0.5 * lightAngle2);

    color = glm::dvec3(1.0) - exp( -20.0/100.0 * (rayleighPhase*rayleighColor + miePhase*mieColor) );
    return color;
}

void Sun::update(Atmosphere &atm){
    calcSunPosition();
    lightDirectionVector = polarToVector();
    lightColor = calcSunColor(atm);
}

glm::vec4 Sun::getLightVector(){
    // return glm::vec4(0,1,0,0);
    float inDeg = zenithAngle*toDeg;
    glm::vec4 out = lightDirectionVector;
    // out.z = -abs(out.z);
    return glm::normalize(out*(1-glm::smoothstep(88.f, 107.f, inDeg)) + glm::vec4(0,0.1,-1,0)*glm::smoothstep(88.f, 107.f, inDeg));
}

glm::vec3 Sun::getColor(){
    float inDeg = zenithAngle*toDeg;
    return lightColor + glm::vec3(0.01f, 0.025f, 0.04f)*glm::smoothstep(95.f, 107.f, inDeg)*0.5f;
    if(inDeg < 88.0) return {1.f, 1.f, 0.8f};
    if(inDeg < 107) return {0.7f, 0.4f, 0.2f};
    return {0.05f, 0.125f, 0.2f};
}
