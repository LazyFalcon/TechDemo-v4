#include "core.hpp"
#include "Servomechanism.hpp"
#include "Yaml.hpp"

namespace {

// assuming range is (-pi, pi]
float differenceBetweenAngles(float a, float b){
    auto diff = fmod(b -a + pi, pi2) - pi; // wrap difference to range -pi;pi
    return diff < -pi ? diff + pi2 : diff;
}

}

std::optional<MinMax> Servomechanism::isAxisLocked(const Yaml& params, int idx){

    MinMax minmax {false, params["Min"][idx].number(), params["Max"][idx].number()};
    if(not params["Limits"][idx].boolean()) return std::none;
    if(not minmax.areMinAndMaxClose()){
        minmax.isSet = true;
        return minmax;
    }
    return minmax;
}

Servomechanism::Servomechanism(const Yaml& moduleParams){
    auto& params = moduleParams["Rotation constriants"];
    // False means free rotation, True and min!= max also, True and min==max disables rotation
    if(auto isx = isAxisLocked(params, 0); isx){

    }

}

void Servomechanism::setTarget(float x, float y, float z){
    if(x) axis.x.target = limit.x.clamp(x);
    if(y) axis.y.target = limit.y.clamp(y);
    if(z) axis.z.target = limit.z.clamp(z);
}

float Servomechanism::move(float diff, float dt){
    return glm::sign(diff) * std::min(abs(diff/dt), vMax);
}
void Servomechanism::run(float dt){
    if(x){
        auto e = differenceBetweenAngles(axis.x.target, axis.x.value);
        axis.x.value += move(e, dt);
    }
    if(y){
        auto e = differenceBetweenAngles(axis.y.target, axis.y.value);
        axis.y.value += move(e, dt);
    }
    if(z){
        auto e = differenceBetweenAngles(axis.z.target, axis.z.value);
        axis.z.value += move(e, dt);
    }
}
glm::mat4 Servomechanism::getTransform(){
    glm::mat4 out(1);
    if(x) out *= glm::rotate(angle, glm::vec3(1,0,0));
    if(y) out *= glm::rotate(angle, glm::vec3(0,1,0));
    if(z) out *= glm::rotate(angle, glm::vec3(0,0,1));

    return out;
}
