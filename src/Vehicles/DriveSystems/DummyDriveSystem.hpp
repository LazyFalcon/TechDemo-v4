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
    btVector3 m_position;
    btVector3 m_lookDirection;
    btVector3 m_moveDirection;
    int m_leadingAxis = 1;
public:
    DummyDriveSystem(VehicleEquipment& eq, btVector3 position) : DriveSystem(eq), m_position(position), m_lookDirection(0,1,0){}

    // void getInputInterface(InputMethods& inputMethods){
        // // * For keyboard + mouse
        // inputMethods.keyboard.push_back([](Input& input){ input.action("W").repeat([this]{m_moveDirection.x=1;}); });
        // inputMethods.keyboard.push_back([](Input& input){ input.action("S").repeat([this]{m_moveDirection.x=-1;}); });
        // inputMethods.keyboard.push_back([](Input& input){ input.action("D").repeat([this]{m_moveDirection.y=1;}); });
        // inputMethods.keyboard.push_back([](Input& input){ input.action("A").repeat([this]{m_moveDirection.y=-1;}); });
        // inputMethods.keyboard.push_back([](Input& input){ input.action("Alt+ScrollUp").on([this]{m_moveDirection.z=1;}); });
        // inputMethods.keyboard.push_back([](Input& input){ input.action("Alt+ScrollDown").on([this]{m_moveDirection.z=-1;}); });
        // // ? da się to połaczyć? chyba nie przy mojej implementacji, hold musiałby się odpalić po czasie
        // Spacebar off []{jump();}
        // Spacebar hold []{jumpMore();}
        // Ctrl+ScrollUp []{speedLimitUp()}
        // Ctrl+ScrollDown []{speedLimitDown()}
        // MouseMove []{m_lookDirection = }

        // // * For x-pad
        // Pad [](float x, y){m_moveDirection.x = x; m_moveDirection.y = y;}
        // inputMethods.xpad.push_back([](Input& input){ input.action("LeftJoy").on([this](float x, y){m_moveDirection.x = x; m_moveDirection.y = y;); });

        // // * For AI, maybe unify with x-pad
        // Movement [](float x, y){m_moveDirection.x = x; m_moveDirection.y = y;}
    // }

    void update(float dt) override;

    void updateInsidePhysicsStep(float dt) override;
};
