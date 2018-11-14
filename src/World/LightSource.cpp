#include "core.hpp"
#include "LightSource.hpp"
#include "RenderDataCollector.hpp"
#include "Yaml.hpp"

LightSource::LightSource(const Yaml& yaml){
    name = yaml["Name"].string();
    setType(yaml["Type"].string());
    readConfig(yaml);
}

LightSource& LightSource::setType(const std::string &type){
    if(type == "POINT") m_type = LightType::Point;
    else if(type == "SPOT") m_type = LightType::Spot;
    else if(type == "AREA") m_type = LightType::Area;
    else if(type == "DIRECT") m_type = LightType::Directional;
    return *this;
}
LightSource& LightSource::setTransform(glm::vec4 position, glm::quat quaternion){
    m_position = position;
    m_transform = glm::translate(position.xyz()) * glm::scale(m_scale);
    return *this;
}
LightSource& LightSource::setTransform(const glm::mat4 &tr){
    m_transform = tr * glm::orientation(m_direction, glm::vec3(0,0,1))* glm::scale(m_scale);
    // m_transform = tr * glm::translate(m_positionOffset.xyz()) * glm::orientation(m_direction, glm::vec3(0,0,1))* glm::scale(m_scale);
    m_position = tr*(glm::vec4(0,0,0,1));
    return *this;
}
LightSource& LightSource::setSpot(float fov, glm::vec4 direction, float startSize){

    return *this;
}
LightSource& LightSource::setArea(glm::vec4 direction, glm::vec4 localUp){

    return *this;
}
LightSource& LightSource::move(const glm::vec4 &v){

    return *this;
}
LightSource& LightSource::move(const glm::mat4 &t){

    return *this;
}
LightSource& LightSource::color(HexColor c){

    return *this;
}
LightSource& LightSource::color(float r, float g, float b){
    m_color = glm::vec4(r,g,b,1);
    return *this;
}
LightSource& LightSource::color(glm::vec3 c){
    m_color = glm::vec4(c,1);
    return *this;
}
LightSource& LightSource::position(const glm::vec4 &p){

    return *this;
}

// TODO: test colision point agains light proxy
void LightSource::cameraInside(const glm::vec4 &eye, float scale){
    auto pos = m_transform*glm::vec4(0,0,0,1);
    auto d = glm::distance(pos, eye);
    if(d > m_falloff.distance + 0.0) m_cameraInside = CameraOutside;
    if(d < m_falloff.distance - 0.0) m_cameraInside = CameraInside;
}
bool LightSource::cull(const Frustum &frustum){
    return true;
}

void LightSource::actionWhenVisible(){
    if(lastFrame==frame()) return; // * to be sure that object will be inserted once per frame :)
    lastFrame = frame();

    RenderDataCollector::lights[0].push_back(this);
    // RenderDataCollector::lights[m_type+m_cameraInside].push_back(this);
}

void LightSource::readConfig(const Yaml& thing){
    switch(m_type){
        case Point:
            break;
        case Spot:
            break;
        case Area:
            break;
    }

    m_energy = thing["Energy"].number();
    m_color = thing["Color"].vec4();
    m_falloff.distance = thing["Falloff_distance"].number();
    setTransform(glm::mat4(
            thing["Position"]["X"].vec30(),
            thing["Position"]["Y"].vec30(),
            thing["Position"]["Z"].vec30(),
            thing["Position"]["W"].vec31()
        ));
}
void LightSource::update(float dt){}
