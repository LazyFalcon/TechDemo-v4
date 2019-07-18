#pragma once

class Yaml;

// for rotation only allowed axes are X and Z, Y is used as targeting one
class InverseKinematics
{
public:
    InverseKinematics(const Yaml& params);

};
