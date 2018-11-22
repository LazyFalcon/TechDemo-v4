#pragma once

class Yaml;

using floats = std::vector<float>;

struct AxisData
{
    float value;
    float min, max;
    int axisId; // * to resolve issues with ordering
    bool limited;
};

// * WS - World Space
// * LS - Local Space
// * kind of strategy
class Joint
{
protected:
    int dof; // * also number of axes used, if axis is blocked, it's not included in dof.
    std::array<AxisData, 3> axes {};
    bool m_ignoreImpossibleTargets {false};
public:
    virtual ~Joint() = default;
    Joint() : Joint(identityMatrix){}
    Joint(const glm::mat4& mat) : m_loc(mat){}

    const glm::mat4& getLoc() const {
        return m_loc;
    }

    enum type {
        RIGID, // * copy transform from parent(with position correction)
        PRISMATIC, // * linear along Z axis
        REVOLUTE, // * rotate around Z axis, targeting along Y axis
        DOF_2, // * rotate around Z and X axis, targeting along Y axis
        DOF_3  // * full yaw pitch roll
    };

    // TODO: in order to IK few joints at once(i.e. mech legs) collect joints and run Inverse transponse on them
    // * IK sets target values in class
    // * usually aims with Y axis to go through target point
    virtual void IK_targetAtPoint(const glm::mat4& parentWS, const glm::vec4& targetPointWS){}
    virtual void toDefaults(){}
    virtual void goToTarget(float miliseconds){}
    virtual glm::mat4 getTransform(){ // * parent WS * transform = moduleTransform
        return glm::mat4(1);
    }
    glm::vec4 pivotWS(){
        return m_loc[4];
    }

    glm::vec4 toBOrigin; // * to A origin is included in loc matrix
    glm::mat4 m_loc; // * local coordinates and also default transform. To get module transform multiply by
};

std::shared_ptr<Joint> createJoint(const Yaml&, glm::vec4);
