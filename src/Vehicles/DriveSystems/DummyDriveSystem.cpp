#include "core.hpp"
#include "DummyDriveSystem.hpp"
#include "Logging.hpp"

/*
* Uproszczony model sterowania droną: drona śledzi punkt(pozycja + orientacja)
* Silnik zajmuje się redukcją wszystkich zewnętrzych sił działających na obiekt(z opóźnieniem, żeby było ładniej)
* Osiąganie punktu odbywa się za pomocą sił, jedynie na podstawie różnicy pozycji(potem jeszcze prędkość i przyspieszenie w połączeniu z predykcją trasy)
*/

class PDReg
{
private:
    float P,D;
    btVector3 m_previousForce {};
public:
    PDReg(float p, float d) : P(p), D(d){}

    btVector3 goTo(btVector3 target, btVector3 current){
        auto err = target - current;

        btVector3 response = P*err + D*(m_previousForce - current);

        m_previousForce = current;
        return response;
    }
};

PDReg pdRegForce = PDReg(0.99, 0.0); // * outputs force
PDReg pdRegPosition = PDReg(0.99, 0.0);  // * outputs impulses
PDReg pdRegTorque = PDReg(0.9, 0.0);  // * outputs torque
PDReg pdRegOrientation = PDReg(0.9, 0.0);  // * outputs impulses

void DummyDriveSystem::update(float dt){
    m_targetPosition += m_moveDirection;
}

void DummyDriveSystem::updateInsidePhysicsStep(float dt){
    btTransform tr;
    eq.rgBody->getMotionState()->getWorldTransform(tr);

    forcePart(dt, tr);
    torquePart(dt, tr);
}

void DummyDriveSystem::forcePart(float dt, btTransform& tr){
    btVector3 externalForces = eq.rgBody->getTotalForce() - m_previouslyappliedForce;

    btVector3 response = -externalForces;
    m_previouslyappliedForce = response;
    // btVector3 response = pdRegForce.goTo(btVector3(0,0,0), externalForces);

    eq.rgBody->applyCentralForce(response);
    eq.rgBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
    if(eq.control.targetPoint){
        auto positionError = convert(*eq.control.targetPoint) - tr.getOrigin();
        auto impulse = pdRegPosition.goTo(btVector3(0,0,0), -positionError)*20;

        eq.rgBody->setLinearVelocity(impulse);
        // eq.rgBody->applyCentralImpulse(impulse);
    }

}
btVector3 QuaternionToEulerXYZ(const btQuaternion &quat)
{
    btVector3 euler;
	btScalar w=quat.getW();	btScalar x=quat.getX();	btScalar y=quat.getY();	btScalar z=quat.getZ();
	double sqw = w*w; double sqx = x*x; double sqy = y*y; double sqz = z*z;
	euler.setZ((atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw))));
	euler.setX((atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))));
	euler.setY((asin(-2.0 * (x*z - y*w))));
    return euler;
}
btVector3 QuaternionToEulerXYZ(const glm::quat &quat)
{
    btVector3 euler;
	btScalar w=quat.w;	btScalar x=quat.x;	btScalar y=quat.y;	btScalar z=quat.z;
	double sqw = w*w; double sqx = x*x; double sqy = y*y; double sqz = z*z;
	euler.setZ((atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw))));
	euler.setX((atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))));
	euler.setY((asin(-2.0 * (x*z - y*w))));
    return euler;
}

void DummyDriveSystem::torquePart(float dt, btTransform& tr){
    btVector3 currentTorque = eq.rgBody->getTotalTorque() - m_previouslyappliedTorque;

    btVector3 response = -currentTorque;
    m_previouslyappliedTorque = response;
    // btVector3 response = pdRegTorque.goTo(btVector3(0,0,0), currentTorque);

    eq.rgBody->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
    eq.rgBody->applyTorque(response);
    if(eq.control.targetDirection){
        // clog("to:", target, "from:", rotation, "by:", delta);
        auto rotation = tr.getRotation();
        // btQuaternion target(convert(*eq.control.targetDirection), 0);
        btQuaternion target(btVector3(1,0,0), 0.1);
        btQuaternion deltaOrientation = target* rotation.inverse();
        btVector3 deltaEuler = QuaternionToEulerXYZ(deltaOrientation);


        auto y = convert(tr.getBasis()[m_leadingAxis]);
        auto tgt = glm::normalize(glm::vec3(-1,-1,1));
        // auto tgt = (*eq.control.targetDirection).xyz();

        auto quat = glm::rotation(y, tgt);
        auto delta = QuaternionToEulerXYZ(quat);

        // You basically get the scaled inverse of the torque you want to apply. Now you "just" need to find an appropriate amount to ease it in.

        // auto rotationError = tr.getBasis()[m_leadingAxis].cross(convert(*eq.control.targetDirection)) + tr.getBasis()[2].cross(btVector3(0,0,1));
        // auto impulse = pdRegOrientation.goTo(btVector3(0,0,0), -rotationError)*5.91;
        clog("tgt:", QuaternionToEulerXYZ(target), "rot:", QuaternionToEulerXYZ(rotation));
        clog("to:", target, "from:", rotation, "by:", deltaEuler, "by:", delta);


        // eq.rgBody->setAngularVelocity(delta);
        eq.rgBody->applyTorqueImpulse(-delta*60);
    }
}


void DummyDriveSystem::provideControlInterfaceForKeyboard(Input& input){
//     input.action("W").repeat([this]{m_moveDirection.x += 0.01;}); });
//     input.action("S").repeat([this]{m_moveDirection.x += -0.01;}); });
//     input.action("D").repeat([this]{m_moveDirection.y += 0.01;}); });
//     input.action("A").repeat([this]{m_moveDirection.y += -0.01;}); });
//     input.action("Alt+ScrollUp").on([this]{m_moveDirection.z += 0.01;}); });
//     input.action("Alt+ScrollDown").on([this]{m_moveDirection.z += -0.01;}); });
}
void DummyDriveSystem::provideControlInterfaceForXPad(Input& input){

}
void DummyDriveSystem::provideControlInterfaceForAI(AiControl& input){
    // * actions are defined in specific AI implementation
    // input.action("Move Forward", [this](glm::vec4 position, glm:vec4 direction){
    //                 m_targetPosition = convert(position, 1);
    //                 m_targetDirection = convert(direction, 0);
    //             });
}

// void DummyDriveSystem::provideControlInterface(ControlInterfaces& interfaces){
        // ? da się to połaczyć? chyba nie przy mojej implementacji, hold musiałby się odpalić po czasie
        // Spacebar off []{jump();}
        // Spacebar hold []{jumpMore();}
        // Ctrl+ScrollUp []{speedLimitUp()}
        // Ctrl+ScrollDown []{speedLimitDown()}
        // MouseMove []{m_lookDirection = }

        // // * For x-pad
        // Pad [](float x, y){m_moveDirection.x = x; m_moveDirection.y = y;}
        // interfaces.xpad.push_back([](Input& input){ input.action("LeftJoy").on([this](float x, y){m_moveDirection.x = x; m_moveDirection.y = y;); });

        // // * For AI, maybe unify with x-pad
        // Movement [](float x, y){m_moveDirection.x = x; m_moveDirection.y = y;}
// }
