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

std::optional<Servomechanism::MinMax> Servomechanism::isAxisLocked(const Yaml& params, int idx) const {

    Servomechanism::MinMax minmax {false, (float)params["Min"][idx].number(), (float)params["Max"][idx].number()};
    if(not params["Limits"][idx].boolean()) return std::nullopt;
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
        axis.x = ValueTarget{0,0};
        limit.x = *isx;
    }
    if(auto isy = isAxisLocked(params, 1); isy){
        axis.y = ValueTarget{0,0};
        limit.y = *isy;
    }
    if(auto isz = isAxisLocked(params, 2); isz){
        axis.z = ValueTarget{0,0};
        limit.z = *isz;
    }

}

void Servomechanism::setTarget(float x, float y, float z){
    if(x) axis.x->target = limit.x.clamp(x);
    if(y) axis.y->target = limit.y.clamp(y);
    if(z) axis.z->target = limit.z.clamp(z);
}

float Servomechanism::move(float diff, float dt) const {
    return glm::sign(diff) * std::min(abs(diff/dt), vMax);
}
void Servomechanism::run(float dt){
    if(axis.x){
        auto e = differenceBetweenAngles(axis.x->target, axis.x->value);
        axis.x->value += move(e, dt);
    }
    if(axis.y){
        auto e = differenceBetweenAngles(axis.y->target, axis.y->value);
        axis.y->value += move(e, dt);
    }
    if(axis.z){
        auto e = differenceBetweenAngles(axis.z->target, axis.z->value);
        axis.z->value += move(e, dt);
    }
}
glm::mat4 Servomechanism::getTransform(){
    glm::mat4 out(1);
    if(axis.x) out *= glm::rotate(axis.x->value, glm::vec3(1,0,0));
    if(axis.y) out *= glm::rotate(axis.y->value, glm::vec3(0,1,0));
    if(axis.z) out *= glm::rotate(axis.z->value, glm::vec3(0,0,1));

    return out;
}
