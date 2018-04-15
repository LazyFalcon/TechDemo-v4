#include "PMK.hpp"
#include <glm/gtx/normal.hpp>
#define BOOST_CHRONO_HEADER_ONLY 1
#include <boost/chrono.hpp>
#include "Constants.hpp"

namespace pmk
{
double period(double val, double a, double b){
    if(val >= b)
        val -= b-a;
    else if(val < a)
        val += b-a;
    return val;
}

double angleDistance(double a, double b){
    auto delta = fmod(b-a, dpi2);
    if(delta < -dpi) return delta + dpi2;
    if(delta > dpi) return delta - dpi2;
    return delta;
}

double mixAngles(double angle, double target, double mix){
    return angleDistance(angle, target) * mix;
}

std::default_random_engine generator;
std::uniform_int_distribution<u32> idistribution(0,0xffffffff);
std::uniform_real_distribution<float> rdistribution(0.0,1.0);
std::normal_distribution<float> ndistribution(0.0,0.4);

u32 irng(){
    return idistribution(generator);
}
u32 irng(u32 min, u32 max){
    std::uniform_int_distribution<u32> d(min, max);
    return d(generator);
}
float rng(){
    return rdistribution(generator);
}
float rng(float min, float max){
    return rdistribution(generator);
}
float gauss(){
    return ndistribution(generator);
}

void init(){
    generator.seed(boost::chrono::high_resolution_clock::now().time_since_epoch().count());

}


float triangleInterpolate(float v00, float v01, float v10, float v11, float u, float v){
    // return v00*(1.f-u)*(1.f-v) + v10*u*(1.f-v) + v01*(1.f-u)*v + v11*u*v;
    // if(u > v) return 500;
    if(u > v) return v00 + u*(v01 - v00) + v*(v10 - v00);
    else return v11 - u*(v11 - v01) - v*(v11 - v10);
}
glm::vec3 triangleNormal(float v00, float v01, float v10, float v11, float u, float v, float gridSize){
    if(u > v){
        glm::vec3 p00(0, 0, v00);
        glm::vec3 p10(gridSize, 0, v10);
        glm::vec3 p01(0, gridSize, v01);
        return glm::normalize(glm::triangleNormal(p01, p00, p10));
    }
    else {
        glm::vec3 p11(gridSize, gridSize, v11);
        glm::vec3 p10(gridSize, 0, v10);
        glm::vec3 p01(0, gridSize, v01);
        return glm::normalize(glm::triangleNormal(p10, p11, p01));
    }

}

glm::mat4 boxAroundAABB(const glm::vec3 &bbMin, const glm::vec3 &bbMax, const glm::vec3 &dir){
    return {};
}
std::function<void(glm::vec4 &bbMin, glm::vec4 &bbMax, glm::vec4 position)> sunSpaceViewBox;
std::function<void(glm::vec4, glm::vec4)> updateSunTransform;

}
