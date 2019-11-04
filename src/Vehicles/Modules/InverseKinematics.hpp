#pragma once

class Yaml;

// for rotation only allowed axes are X and Z, Y is used as targeting one
class InverseKinematics
{
private:
    int rotateAroundAxisIdx;
    bool hasLimits;
    float min, max;
    const int targetingAxisIdx = 1;

public:
    InverseKinematics(const Yaml& params);
    std::array<float, 3> calculate(const glm::mat4& moduleWS, glm::vec4 targetWS) const;
};
