#include "core.hpp"
#include "LightSource.hpp"

LightSource& LightSource::setType(const std::string &type){
    if(type == "POINT"){
        m_type = LightType::Point;
    }
    return *this;
}
LightSource& LightSource::setTransform(glm::vec4 position, glm::quat quaternion){
    m_position = position;
    m_transform = glm::translate(position.xyz()) * glm::scale(m_scale);
    return *this;
}
LightSource& LightSource::setTransform(const glm::mat4 &tr){
    m_transform = tr * glm::translate(m_positionOffset.xyz()) * glm::orientation(m_direction, glm::vec3(0,0,1))* glm::scale(m_scale);
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

int LightSource::cameraInside(const glm::vec4 &eye, float scale){
    auto pos = m_transform*glm::vec4(0,0,0,1);
    auto d = glm::distance(pos, eye);
    if(d > m_fallof + 0.0) return 1;
    if(d < m_fallof - 0.0) return -1;
    return 0;
}
bool LightSource::cull(const Frustum &frustum){
    return true;
}
