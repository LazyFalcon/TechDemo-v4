/**
 *  local Z is light direction
 *  blender light equations and properties: https://www.blender.org/manual/render/blender_render/lighting/lights/light_attenuation.html
 *
 *  lightSource is controlled by owner,
 *  sources are egistered in specific container, from there could be culled

 *  calculations are likely cheap, so i think at this stage we dont need to use stencil buffer to reduce overdraw,
 *  only face culling
 */
#pragma once
#include "common.hpp"
#include "GPUResources.hpp"

class Camera;
class Frustum;

struct LightSource
{
    enum LightType
    {
        Point, Spot, Area,
    };
    glm::mat4 m_transform {};
    glm::vec4 m_position {};
    glm::vec4 m_color {};
    glm::vec3 m_direction {0,0,1};
    glm::vec3 m_scale {};
    glm::vec4 m_positionOffset {};
    float m_energy {};
    float m_fallof {};
    float m_cosAngle {};
    bool isShadowCaster {false};
    LightType m_type;

    // sclae cone and move center to reach start size
    LightSource(LightType type) : m_type(type){}
    LightSource(const std::string &type) : m_type(getType(type)){}
    LightSource& setType(const std::string &type);
    LightSource& setTransform(glm::vec4 position, glm::quat quaternion);
    LightSource& setTransform(const glm::mat4 &tr);
    LightSource& setSpot(float fov, glm::vec4 direction, float startSize);
    LightSource& setArea(glm::vec4 direction, glm::vec4 localUp);
    LightSource& move(const glm::vec4 &v);
    LightSource& move(const glm::mat4 &t);
    LightSource& color(HexColor c);
    LightSource& color(float r, float g, float b);
    LightSource& color(glm::vec3 c);
    LightSource& position(const glm::vec4 &p);
    static LightType getType(const std::string &type){
        if(type == "Point") return Point;
        if(type == "Spot") return Spot;
        if(type == "Area") return Area;
        return Point;
    }

    int cameraInside(const glm::vec4 &eye, float scale = 1.f);
    bool cull(const Frustum &frustum);
};

struct LightSourcesContainer
{
    std::map<LightSource::LightType, std::list<std::shared_ptr<LightSource>>> lights;

    std::shared_ptr<LightSource> emplace(LightSource::LightType type){
        lights[type].emplace_back(std::make_unique<LightSource>(type));
        return lights[type].back();
    }
    std::shared_ptr<LightSource> emplace(const std::string &type){
        lights[LightSource::getType(type)].emplace_back(std::make_unique<LightSource>(type));
        return lights[LightSource::getType(type)].back();
    }
    auto begin(){
        return lights.begin();
    }
    auto end(){
        return lights.end();
    }
    void remove(std::shared_ptr<LightSource> &light){
        lights[light->m_type].remove(light);
    }
    void update(float dt);

};
