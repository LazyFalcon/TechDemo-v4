#pragma once
#include "IModule.hpp"

class Turret : public IModule
{
private:
    int m_targetIndex;
    const glm::vec4& getTarget(){
        return vehicle.fireControlUnit->getTarget(m_targetIndex);
    }
public:
    Turret(const std::string& name, Vehicle &vehicle, IModule* parent) : IModule(name, vehicle, parent){
        m_targetIndex = vehicle.fireControlUnit->idForTurret();
    }
    void update(float dt) override {

        // auto target = vehicle.vehicleControlUnit->getTarget();

        // joint->IK_targetAtPoint(getTransform(), target);

        this->transform(joint->getTransform());
    }
};

class GunServo : public IModule
{
private:
    int m_targetIndex;
    const glm::vec4& getTarget(){
        return vehicle.fireControlUnit->getTarget(m_targetIndex);
    }
public:
    GunServo(const std::string& name, Vehicle &vehicle, IModule* parent) : IModule(name, vehicle, parent){
        m_targetIndex = vehicle.fireControlUnit->idForGunServo();
    }
    void update(float dt) override {

        // auto target = vehicle.vehicleControlUnit->getTarget();

        // joint->IK_targetAtPoint(getTransform(), target);

        this->transform(joint->getTransform());
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
        this->transform(joint->getTransform());
    }
    std::string resource;
};
