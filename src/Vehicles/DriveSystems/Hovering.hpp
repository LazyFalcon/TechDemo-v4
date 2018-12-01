#pragma once


class ThrustGenerator
{
private:
    btVector3 m_generatedForce;
    btVector3 m_generatedTorque;
    struct {p, i, d} params {1, 0, 0};
public:
    auto generate(btVector3 force, btVector3 torque){ // ! timestep jest potrzebny!
        auto fe = force - m_generatedForce;
        auto te = torque - m_generatedTorque;

        // inertia and pid equations here
        m_generatedForce += fe*0.3f;
        m_generatedTorque += te*0.3f;

        return std::make_pair(m_generatedForce, m_generatedTorque);
    }
};

class HoverEngine
{
private:
    btVector3 m_moveDirection;
    float m_targetHeight;
    float m_temperature;
    float m_boosterCapacity;
    bool m_isJumping;
    bool m_boostersInUse;
    ThrustGenerator m_thrustGenerator;
public:

    void getInputInterface(){
        // * For keyboard + mouse
        W repeat []{m_moveDirection.x=1;}
        S repeat []{m_moveDirection.x=-1;}
        D repeat []{m_moveDirection.y=1;}
        A repeat []{m_moveDirection.y=-1;}
        Alt+C+ScrollUp on []{m_targetHeight.y=-1;}
        Alt+C+ScrollDown on []{m_targetHeight.y=1;}
        // ? da się to połaczyć? chyba nie przy mojej implementacji, hold musiałby się odpalić po czasie
        Spacebar off []{jump();}
        Spacebar hold []{jumpMore();}
        Ctrl+ScrollUp []{speedLimitUp()}
        Ctrl+ScrollDown []{speedLimitDown()}

        // * For x-pad
        Pad [](float x, y){m_moveDirection.x = x; m_moveDirection.y = y;}

        // * For AI, maybe unify with x-pad
        Movement [](float x, y){m_moveDirection.x = x; m_moveDirection.y = y;}
    }

    void update(){


    }
    // ? pytanie: czy wiliczamy siły już z bezwładnością silnika, czy wyliczamy potrzebny kierunek siły, a silnik zajmuje sięwygenerowaniem jej i ukierunkowaniem?
    // ? to drugie wygląda ciekawiej

    auto keepInPlace(btVector3 currentForce, btVector3 currentTorque){
        // TODO: calculate delta in position and rotation from current frame, apply force and torque
        return std::make_pair(-worldForces, -currentTorque;
    }
    btVector3 forceToKeepHeight(float height){}
    btVector3 jumpForce(btVector3 worldForces){}
    btVector3 forceToMove(btVector3 worldForces){}

    void updateInsidePhysicsStep(){
        // ? from origin or attach point?
        auto raycastAlong = eq.physics.closesetHit(eq.btMat.getOrigin(), eq.btMat.getOrigin()+eq.btMat[2]*100.f);
        auto raycastDown = eq.physics.closesetHit(attachPointWS, eq.btMat.getOrigin()+btVector3(0,0,-1)*100.f);
        auto distanceToDown = parseRaycast();

        auto currentForce = eq.rgBody.getTotalForce();
        auto currentTorque = eq.rgBody.getTotalTorque();
        auto [force, torque] = keepInPlace(currentForce, currentTorque); // ? czy bierze pod uwagę to co aplikowaliśmy w poprzedniej ramce? Sprawdź
        force += forceToKeepHeight(distanceToDown); // * tu jakiś regulator na pewno
        force += jumpForce(); // * tu jakiś regulator na pewno
        force += forceToMove(); // * tu jakiś regulator na pewno

        torque += keepUpRight(eq.btMat);
        torque += rotate();

        auto [forceToApply, torqueToApply] = m_thrustGenerator.generate(force, torque);
    }

};