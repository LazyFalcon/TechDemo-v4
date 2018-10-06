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
#include "GPUResources.hpp"
#include "SceneObject.hpp"

class Camera;
class Frustum;
class Yaml;
// * https://docs.blender.org/manual/en/dev/render/blender_render/lighting/lights/attenuation.html
struct LightSource : public ObjectInterface
{
    enum LightType
    {
        Point=0, Spot, Area, Directional, LightTypeLast
    };
    enum CameraRelation
    {
        CameraInside=0, CameraOutside=LightTypeLast
    };

    enum class Falloff
    {
        Linear, Quadratic, LinearxQuad, InverseSquare, InverseLinear
    };

    /*
    TODO: Check if it wouldn't be easier to use linear image to sample falloff? many of them can be mixed at once
    TODO: Area lights, Line lights, how to incorporate them?
    */
    struct {
        Falloff type;
        float constant;
        float linear;
        float quadric;
        float distance;
        bool isLimitedToSphere;

        float getLinearXQuad(float r){
            return (distance*distance)/(distance*distance + quadric*r*r) * distance/(distance + linear*r);
        }

        float getInverse(float r){
            return 1.f/(quadric*r*r + linear*r + constant);
        }

        float apllySphereLimitConditionally(float intensity, float r){
            if(isLimitedToSphere and r < distance) return intensity * (distance - r)/distance;
            else if(isLimitedToSphere and r >= distance) return 0;
            else return intensity;
        }

    } falloff {};

    float getIntensity(float rayLenght){
        if(falloff.type == Falloff::Linear or falloff.type == Falloff::Quadratic or falloff.type == Falloff::LinearxQuad){
            return m_energy * falloff.getLinearXQuad(rayLenght);
        }
        else {
            return m_energy * falloff.getInverse(rayLenght);
        }
    }

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
    CameraRelation m_cameraInside;
    bool castShadows {false};
    std::string name;

    btRigidBody* collider {nullptr};

    LightSource(const Yaml&);
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
    static LightType getType(std::string p_type){
        std::transform(p_type.begin(), p_type.end(), p_type.begin(), [](char c){ return std::tolower(c); });
        if(p_type == "point") return Point;
        if(p_type == "spot") return Spot;
        if(p_type == "area") return Area;
        return Point;
    }

    void readConfig(const Yaml& yaml);

    void cameraInside(const glm::vec4 &eye, float scale = 1.f);
    bool cull(const Frustum &frustum);
    void actionWhenVisible() override;
    void update(float dt);
};
