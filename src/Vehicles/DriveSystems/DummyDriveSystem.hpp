#pragma once
#include "DriveSystem.hpp"

/*
! Zamiast obracać pojazdem w niewiadaomym kierunku i pozycji, wyznaczyć kierunek i pozycję którą ten powinien utrzymywać
* może to i gorsze rozwiązanie z punktu widzenia symulacji, ale całkiem wygodne i stabilne
* po prostu poruszamy wirtualną kulką
*/
class DummyDriveSystem : public DriveSystem
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
    DummyDriveSystem(VehicleEquipment& eq, btVector3 position) : DriveSystem(eq), m_targetPosition(position), m_lookDirection(0,1,0){}
    void provideControlInterfaceForKeyboard(Input& input) override;
    void provideControlInterfaceForXPad(Input& input) override;
    void provideControlInterfaceForAI(AiControl& input) override;

    // void getInputInterface(InputMethods& inputMethods){

    // }

    void update(float dt) override;

    void updateInsidePhysicsStep(float dt) override;
};
