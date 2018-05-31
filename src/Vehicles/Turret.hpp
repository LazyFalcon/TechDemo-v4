#pragma once
#include "IModule.hpp"

struct Joint
{
    glm::vec4 fromAOrigin;
    glm::vec4 toBOrigin;

    float min, max;
    int axis;

    float value;
};

/*
* dummy module responsible mainly for handling default module behaviors
*/
class TurretPart : public IModule
{


};

class Weapon;
using TurretKinematicLink = std::array<Joint, 2>;

/*
* class that handles turrets: kinematic module with many guns, handles whole logic internally
* for now max two subturrets and one aimpoint, for more complicated setups go fuck yourself
* make two turrets connected via platform, each turret reports to panel minimal and maximal platform movement that will allow to aim properly
*/
class Turret : public IModule
{
private:
    std::vector<std::shared_ptr<Weapon>> m_installedWeapons;
    TurretKinematicLink m_kinematicLinks;
public:

};

/*
* turret implementation with two attached heads, little different way of calculating IK:
* base of turret always rotate in direction of target, I assumed that heads are somehow symmetrical
*/
class MultiHeadTurret : public IModule
{
private:
    TurretKinematicLink m_linkA;
    TurretKinematicLink m_linkB;
    Joint base;
public:

};
