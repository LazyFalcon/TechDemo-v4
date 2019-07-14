#pragma once
#include "VehicleControlSystem.hpp"

/*
! Zamiast obracać pojazdem w niewiadaomym kierunku i pozycji, wyznaczyć kierunek i pozycję którą ten powinien utrzymywać
* może to i gorsze rozwiązanie z punktu widzenia symulacji, ale całkiem wygodne i stabilne
* po prostu poruszamy wirtualną kulką
*/
class DroneLikeControl : public VehicleControlSystem
{
private:
    btVector3 m_targetPosition;
    btVector3 m_targetDirection;
    btVector3 m_lookDirection;
    btVector3 m_moveDirection;
    int m_leadingAxis = 1;

    btVector3 m_previouslyappliedForce {};
    btVector3 m_previouslyappliedTorque {};

    void forcePart(float dt, btTransform& tr);
    void torquePart(float dt, btTransform& tr);

public:
    DroneLikeControl(Vehicle& eq, btVector3 position) : VehicleControlSystem(eq), m_targetPosition(position), m_lookDirection(0,1,0){}

    void update(float dt) override;

    void updateInsidePhysicsStep(float dt) override;
};
