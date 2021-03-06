#pragma once
#include "VehicleControlSystem.hpp"

class PhysicalWorld;
/*
! Zamiast obracać pojazdem w niewiadaomym kierunku i pozycji, wyznaczyć kierunek i pozycję którą ten powinien utrzymywać
* może to i gorsze rozwiązanie z punktu widzenia symulacji, ale całkiem wygodne i stabilne
* po prostu poruszamy wirtualną kulką
*/
class DroneLikeControl : public VehicleControlSystem
{
private:
    enum State
    {
        Moving,
        Breaking,
        Steady
    };
    State state {Steady};
    PhysicalWorld& m_physics;
    btVector3 m_virtualPosition {};
    btVector3 m_virtualDirection {};
    float m_velocity {};
    float m_velocityCap;
    float m_acceleeration;
    std::optional<float> m_heightAboveSurface;

    btVector3 m_lastAppliedTorqueImpulse {};

    btGeneric6DofConstraint* m_constraint;

    float m_prevYAngle {0.f};
    float m_prevZAngle {0.f};

    btVector3 m_targetPosition;
    btVector3 m_targetDirection;
    btVector3 m_lookDirection;
    btVector3 m_moveDirection;
    int m_leadingAxis = 1;

    btVector3 m_previouslyappliedForce {};
    btVector3 m_previouslyappliedTorque {};
    void computeState();
    btVector3 getMoveDirection(glm::vec4 control, const btTransform&) const;
    float accelerationAccordingToState() const;

    void adjustTargetHeightIfNeeded(btVector3&);
    void adjustDirection(btVector3&);
    btVector3 getDesiredAngles(const btVector3&, const btVector3&, const btVector3&);
    void positionPart(float dt, const btTransform& tr);
    void orientationPart(float dt, const btTransform& tr);

public:
    DroneLikeControl(PhysicalWorld& physics, Vehicle& eq, btVector3 position);
    ~DroneLikeControl();

    void update(float dt) override;

    void updateInsidePhysicsStep(float dt) override;
};
