#include "core.hpp"
#include "Joint.hpp"
#include "Yaml.hpp"
/*
! rules:
* if limits are not set at all -> connection is rigid
* if limit is true, but distance between min and max is close to 0 this axis is blocked
* if limit is false, axis has full free movement
* if limit is true and values are different, joint value in this axis is limited to this values
! limits apply to local axes, so if you want to have object rotated in default position you have to rotate it to have different LOC
*/

namespace {
    struct LimitValues {
        float min, max;
    };
    using Limits = std::optional<LimitValues>;

    auto getAngleToReachTarget(float from, float to, const Limits& limits){
        float e1 = to - from;
        float e2 = to - from - 2*pi;

        bool isPossible = not limits ? true : (to > limits->min and to < limits->max);

        return std::make_tuple(std::abs(e1) < std::abs(e2) ? e1 : e2, isPossible);
    }

    auto rotateAndKeepIn0To2piRange(float value, float by){
        value += by;
        if(value > 2*pi) value -= 2*pi;
        else if(value < 0) value += 2*pi;

        return value;
    }
}

class Rigid : public Joint
{
public:
    Rigid(const Yaml& config){}
    void IK_targetAtPoint(const glm::mat4& parentWS, const glm::vec4& targetPointWS) override {}
    void toDefaults() override {}
    void goToTarget(float miliseconds) override {}
    glm::mat4 getTransform(){
        return m_loc;
    }
};

class Revolute : public Joint
{
private:
    // TODO: put this all in structures
    Limits m_limits;
    float m_position, m_defaultPosition, m_targetPosition;
    float m_velocity; // * rads/milis
public:
    Revolute(const Yaml& config){
        m_defaultPosition = 0; // TODO: read from config
        m_targetPosition = 0;
        m_position = m_defaultPosition;
        m_velocity = 10;
    }

    void IK_targetAtPoint(const glm::mat4& parentWS, const glm::vec4& targetPointWS) override {

    }
    void toDefaults() override {
        m_targetPosition = m_defaultPosition;
    }
    void goToTarget(float miliseconds) override {
        auto [e, possibleToReachTarget] = getAngleToReachTarget(m_position, m_targetPosition, m_limits);

        if(not possibleToReachTarget and not m_ignoreImpossibleTargets) return;
        e = glm::sign(e) * std::min(abs(e/miliseconds), m_velocity);
        m_position = rotateAndKeepIn0To2piRange(m_position, e);
    }
    glm::mat4 getTransform(){
        // return m_loc * glm::rotate(m_position, glm::vec3(0,0,1)) * glm::translate(toBOrigin.xyz());
        return m_loc * glm::translate(toBOrigin.xyz());
    }
};

std::unique_ptr<Joint> createJoint(const Yaml& config, glm::vec4 fromSocketToOrigin){
    std::unique_ptr<Joint> out;
    if(config["Type"] == "Rigid") out = std::make_unique<Rigid>(config);
    if(config["Type"] == "Revolute") out = std::make_unique<Revolute>(config);

    out->toBOrigin = fromSocketToOrigin;
    out->m_loc = glm::mat4(config["X"].vec30(),config["Y"].vec30(),config["Z"].vec30(),config["W"].vec31());
    console.log("joint:", out->toBOrigin, out->m_loc[3]);
    return out;
}
