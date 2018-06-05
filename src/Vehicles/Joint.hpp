#pragma once
#include "common.hpp"

using floats = std::vector<float>;

struct AxisData
{
    float value;
    float min, max;
    int axisId; // * to resolve issues with ordering
    bool limited;
};

// * kind of strategy
class Joint
{
private:
    void (Joint::*m_solver)(const glm::vec4&);

    void dof_0_solver(const glm::vec4& targetInLocaLspace){}
    void dof_1_solver(const glm::vec4& targetInLocaLspace){}
    void dof_2_solver(const glm::vec4& targetInLocaLspace){}
    void dof_3_solver(const glm::vec4& targetInLocaLspace){}


    int dof; // * also number of axes used, if axis is blocked, it's not included in dof.
    std::array<AxisData, 3> axes {};
    glm::mat4 m_loc; // * local coordinates and also default transform. To get module transform multiply by

public:
    Joint() : Joint(identityMatrix){}
    Joint(const glm::mat4& mat) : m_loc(mat){
        m_solver = &Joint::dof_0_solver;
    }

    const glm::mat4& loc() const {
        return m_loc;
    }

    enum type {
        RIGID, // * copy transform from parent(with position correction)
        HINGE, // * rotate around axis within limitss
        DOF_2, // * full sphere without roll, mainly around axis and one parallel to axis
        DOF_3  // * full yaw pitch roll
    };

    glm::vec4 toBOrigin;

    void compileLimits(floats hasThe, floats minValues, floats maxValues);
    void setRigidConnection();
};
