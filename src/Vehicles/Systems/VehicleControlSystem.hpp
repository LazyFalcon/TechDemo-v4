#pragma once

class Vehicle;

class VehicleControlSystem
{
public:
    std::string name;
    Vehicle &vehicle;

    VehicleControlSystem(Vehicle &eq): vehicle(eq){}
    virtual ~VehicleControlSystem() = default;
    virtual void update(float dt) = 0;
    virtual void updateInsidePhysicsStep(float dt) = 0;

};
