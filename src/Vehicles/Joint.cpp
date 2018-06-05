#include "Joint.hpp"
/*
! rules:
* if limits are not set at all -> connection is rigid
* if limit is true, but distance between min and max is close to 0 this axis is blocked
* if limit is false, axis has full free movement
* if limit is true and values are different, joint value in this axis is limited to this values
! limits apply to local axes, so if you want to have object rotated in default position you have to rotate it to have different LOC
*/
void Joint::compileLimits(floats hasThe, floats minValues, floats maxValues){
    dof = RIGID;

    for(int a=0; a<3; a++){
        axes[dof].limited = false;
        axes[dof].axisId = a;

        if(hasThe[a] == 0.f){
            dof++;
            continue;
        }

        axes[dof].limited = true;
        axes[dof].min = minValues[a];
        axes[dof].max = maxValues[a];
        if(abs(minValues[a] - maxValues[a]) > 0.001f) dof++;
    }

    if(dof == 1) m_solver = &Joint::dof_1_solver;
    if(dof == 2) m_solver = &Joint::dof_2_solver;
    if(dof == 3) m_solver = &Joint::dof_3_solver;
}

void Joint::setRigidConnection(){
    dof = RIGID;
}
