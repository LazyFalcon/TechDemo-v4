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
    enum State {Moving, Breaking, Steady};
    State state {Steady};

    btVector3 m_virtualPosition {};
    btVector3 m_virtualDirection {};
    float m_velocity {};
    float m_velocityCap;
    float m_acceleeration;

    btVector3 m_targetPosition;
    btVector3 m_targetDirection;
    btVector3 m_lookDirection;
    btVector3 m_moveDirection;
    int m_leadingAxis = 1;

    btVector3 m_previouslyappliedForce {};
    btVector3 m_previouslyappliedTorque {};
    void computeState();
    float accelerationAccordingToState() const;

    void positionPart(float dt, btTransform& tr);
    void orientationPart(float dt, btTransform& tr);

public:
    DroneLikeControl(Vehicle& eq, btVector3 position) : VehicleControlSystem(eq), m_virtualPosition(position), m_lookDirection(0,1,0){}

    void update(float dt) override;

    void updateInsidePhysicsStep(float dt) override;
};
