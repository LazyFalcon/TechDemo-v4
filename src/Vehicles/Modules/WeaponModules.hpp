#pragma once
#include "IModule.hpp"
#include "Servomechanism.hpp"
#include "InverseKinematics.hpp"

// local Y axis is used as look direction
class Yaml;
class Turret : public IModule
{
private:
    int m_targetIndex;
    Servomechanism servo;
    InverseKinematics ik;
    const glm::vec4& getTarget(){
        return vehicle.fireControlUnit->getTarget(m_targetIndex);
    }
public:
    Turret(const std::string& name, Vehicle &vehicle, IModule* parent, const Yaml& yaml) :
        IModule(name, vehicle, parent),
        servo(yaml),
        ik(yaml)
    {
        m_targetIndex = vehicle.fireControlUnit->idForTurret();
    }
    void update(float dt) override {
        auto [x,y,z] = ik.calculate(getTransform(), vehicle.fireControlUnit->getTarget(m_targetIndex));
        servo.updateTarget(x,y,z);
        servo.run(dt);
        this->transform(servo.getTransform());
    }
};

class GunServo : public IModule
{
private:
    int m_targetIndex;
    Servomechanism servo;
    InverseKinematics ik;
    const glm::vec4& getTarget(){
        return vehicle.fireControlUnit->getTarget(m_targetIndex);
    }
public:
    GunServo(const std::string& name, Vehicle &vehicle, IModule* parent, const Yaml& yaml) :
        IModule(name, vehicle, parent),
        servo(yaml),
        ik(yaml)
    {
        m_targetIndex = vehicle.fireControlUnit->idForGunServo();
    }
    void update(float dt) override {
        auto [x,y,z] = ik.calculate(getTransform(), vehicle.fireControlUnit->getTarget(m_targetIndex));
        servo.updateTarget(x,y,z);
        servo.run(dt);
        this->transform(servo.getTransform());
    }
};

class Gun : public IModule
{
private:
    int m_targetIndex;
    const glm::vec4& getTarget(){
        return vehicle.fireControlUnit->getTarget(m_targetIndex);
    }
public:
    Gun(const std::string& name, Vehicle &vehicle, IModule* parent) : IModule(name, vehicle, parent){
        m_targetIndex = vehicle.fireControlUnit->idForGun(reinterpret_cast<u64>(parent));
    }
    void update(float dt) override {
        this->transform(glm::mat4(1));
    }
    std::string resource;
};
