#include "core.hpp"
#include "Servomechanism.hpp"
#include "Yaml.hpp"

namespace
{
// assuming range is (-pi, pi]
float differenceBetweenAngles(float a, float b) {
    auto diff = fmod(b - a + pi, pi2) - pi; // wrap difference to range -pi;pi
    return diff < -pi ? diff + pi2 : diff;
}

}

std::optional<Servomechanism::ValueTarget> Servomechanism::retrieveAxis(const Yaml& params, int idx) const {
    std::optional<Servomechanism::ValueTarget> out;
    if(not params["Axis"][idx].boolean())
        return out;

    out.emplace(0, 0.002f);
    // out.emplace(params["Value"][idx].number(), 0.002f);

    // if(params["Limits"][idx].boolean())
    //     out->limit.emplace(params["Min"][idx].number(), params["Max"][idx].number());

    return out;
}

Servomechanism::Servomechanism(const Yaml& moduleParams) {
    auto& params = moduleParams["Rotation constriants"];
    axis.x = retrieveAxis(params, 0);
    axis.y = retrieveAxis(params, 1);
    axis.z = retrieveAxis(params, 2);
}

void Servomechanism::updateTarget(float x, float y, float z) {
    if(axis.x)
        axis.x->setTarget(axis.x->value + x);
    if(axis.y)
        axis.y->setTarget(axis.y->value + y);
    if(axis.z)
        axis.z->setTarget(axis.z->value + z);
}
void Servomechanism::setTarget(float x, float y, float z) {
    if(axis.x)
        axis.x->setTarget(x);
    if(axis.y)
        axis.y->setTarget(y);
    if(axis.z)
        axis.z->setTarget(z);
}

float Servomechanism::move(float diff, float dt) const {
    return glm::sign(diff) * std::min(abs(diff / dt), vMax);
}
void Servomechanism::run(float dt) {
    if(axis.x) {
        auto e = differenceBetweenAngles(axis.x->target, axis.x->value);
        axis.x->value += move(e, dt);
    }
    if(axis.y) {
        auto e = differenceBetweenAngles(axis.y->target, axis.y->value);
        axis.y->value += move(e, dt);
    }
    if(axis.z) {
        auto e = differenceBetweenAngles(axis.z->target, axis.z->value);
        axis.z->value += move(e, dt);
    }
}
glm::mat4 Servomechanism::getTransform() {
    glm::mat4 out(1);
    if(axis.x)
        out *= glm::rotate(axis.x->value, glm::vec3(1, 0, 0));
    if(axis.y)
        out *= glm::rotate(axis.y->value, glm::vec3(0, 1, 0));
    if(axis.z)
        out *= glm::rotate(axis.z->value, glm::vec3(0, 0, 1));

    return out;
}
