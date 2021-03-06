#include "core.hpp"
#include "InverseKinematics.hpp"
#include <glm/gtx/vector_angle.hpp>
#include "Logger.hpp"
#include "Yaml.hpp"

InverseKinematics::InverseKinematics(const Yaml& moduleParams) {
    const auto& params = moduleParams["Rotation constriants"];
    for(auto idx : {0, 2}) {
        if(params["Axis"][idx].boolean()) {
            rotateAroundAxisIdx = idx;
            hasLimits = params["Limits"][idx].boolean();
            min = params["Min"][idx].number();
            max = params["Max"][idx].number();
            // console.log("Selected axis", idx);
        }
    }
}

std::array<float, 3> InverseKinematics::calculate(const glm::mat4& moduleWS, glm::vec4 targetWS) const {
    const auto targetingAxis = moduleWS[targetingAxisIdx].xyz();
    const auto rotateAroundAxis = moduleWS[rotateAroundAxisIdx].xyz();

    auto target =
        targetWS.xyz()
        - moduleWS[3].xyz(); // subtract position of module from target point to get correct distance to plane,
                             // assumption is that rotation point is in center of the world
    const auto targetDistanceTo = glm::dot(target, rotateAroundAxis);

    target -= rotateAroundAxis * targetDistanceTo; // point is now on the plane of rotation
    target = glm::normalize(target);               // target becomes vector pointing on point

    const auto angle = glm::orientedAngle(target, targetingAxis, rotateAroundAxis);
    // console.flog(rotateAroundAxisIdx,targetWS, angle, target, targetingAxis, rotateAroundAxis);

    std::array<float, 3> out {};
    out[rotateAroundAxisIdx] = isnormal(angle) ? angle : 0;
    return out;
}
