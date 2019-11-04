#pragma once

class Input;
class Vehicle;

/*
 You need to learn about separation of concerns.

 You need to write a player controller script that takes inputs and calls run/shoot in a separate script attached to the player.

 You then need the run/shoot/jump scripts, either together as an action controller or separate, doesn't matter. Every entity that can do one of these gets the script/s.

 Then your enemies get an aicontroller separate script that calls the run shoot jump code after making decisions.

 Three parts, each with different jobs.

 Playercontroller turns inputs into commands.
 Aicontroller turns statemachine into commands Action controller turns commands onto actions

 --

 Controller have to provide something that translates hihg level AI states to low level actions
 For player just fill input
 Common things and shared states must be setup in player code

 */

class VehicleControlSystem
{
public:
    std::string name;
    Vehicle& vehicle;

    VehicleControlSystem(Vehicle& eq) : vehicle(eq) {}
    virtual ~VehicleControlSystem() = default;
    virtual void update(float dt) = 0;
    virtual void updateInsidePhysicsStep(float dt) = 0;
};
