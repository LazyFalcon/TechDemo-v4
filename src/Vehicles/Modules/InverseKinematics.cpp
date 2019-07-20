#include "core.hpp"
#include "InverseKinematics.hpp"
#include "Yaml.hpp"
#include "Logger.hpp"
#include <glm/gtx/vector_angle.hpp>

InverseKinematics::InverseKinematics(const Yaml& moduleParams){
    const auto& params = moduleParams["Rotation constriants"];
    for(auto idx : {0,2}){
        if(params["Axis"][idx].boolean()){
            rotateAroundAxisIdx = idx;
            hasLimits = params["Limits"][idx].boolean();
            min = params["Min"][idx].number();
            max = params["Max"][idx].number();
            console.log("Selected axis", idx);
        }
    }
}
int guard = 0;

std::array<float, 3> InverseKinematics::calculate(const glm::mat4& moduleWS, glm::vec4 targetWS) const {
    const auto targetingAxis = moduleWS[targetingAxisIdx].xyz();
    const auto rotateAroundAxis = moduleWS[rotateAroundAxisIdx].xyz();

    auto target = targetWS.xyz() - moduleWS[3].xyz(); // subtract position of module from target point to get correct distance to plane,
                             // assumption is that rotation point is in center of the world
    const auto targetDistanceTo = glm::dot(target, rotateAroundAxis);

    target -= rotateAroundAxis*targetDistanceTo; // point is now on the plane of rotation
    target = glm::normalize(target); // target becomes vector pointing on point

    const auto angle = glm::orientedAngle(target, targetingAxis, rotateAroundAxis);
    if(guard < 6){
        guard++;
        console.log(rotateAroundAxisIdx, angle, target, targetingAxis, rotateAroundAxis);
    }

    std::array<float, 3> out {};
    out[rotateAroundAxisIdx] = angle;
    return out;
}
