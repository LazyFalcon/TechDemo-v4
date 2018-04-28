#pragma once
#include "common.hpp"


namespace pmk
{
template<typename T>
bool inRange(T value, T bottom, T top){
    return bottom <= value or value < top;
}

double period(double val, double a, double b);
double mixAngles(double val, double a, double b);

u32 irng();
u32 irng(u32 min, u32 max);
float rng();
float gauss();
float rng(float min, float max);

void init();

template<typename T>
T bilinearInterpolate(T v00, T v01, T v10, T v11, float u, float v){
    return v00*(1.f-u)*(1.f-v) + v10*u*(1.f-v) + v01*(1.f-u)*v + v11*u*v;
}
float triangleInterpolate(float v00, float v01, float v10, float v11, float u, float v);
glm::vec3 triangleNormal(float v00, float v01, float v10, float v11, float u, float v, float gridSize);

inline glm::vec3 min(const glm::vec3 &a, const glm::vec3 &b){
    return glm::vec3(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z)
        );
}
inline glm::vec3 max(const glm::vec3 &a, const glm::vec3 &b){
    return glm::vec3(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z)
        );
}

inline glm::vec4 min(const glm::vec4 &a, const glm::vec4 &b){
    return glm::vec4(
            std::min(a.x, b.x),
            std::min(a.y, b.y),
            std::min(a.z, b.z),
            std::min(a.w, b.w)
        );
}
inline glm::vec4 max(const glm::vec4 &a, const glm::vec4 &b){
    return glm::vec4(
            std::max(a.x, b.x),
            std::max(a.y, b.y),
            std::max(a.z, b.z),
            std::max(a.w, b.w)
        );
}

glm::mat4 boxAroundAABB(const glm::vec3 &bbMin, const glm::vec3 &bbMax, const glm::vec3 &dir);

// transform position to sun light space and calc min and max in that space
extern std::function<void(glm::vec4 &bbMin, glm::vec4 &bbMax, glm::vec4 position)> sunSpaceViewBox;
extern std::function<void(glm::vec4, glm::vec4)> updateSunTransform;

}
