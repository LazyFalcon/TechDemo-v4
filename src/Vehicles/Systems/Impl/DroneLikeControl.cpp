#include "core.hpp"
#include "DroneLikeControl.hpp"
#include "input.hpp"
#include "Logger.hpp"
#include "Vehicle.hpp"
#include "Utils.hpp"
#include "PhysicalWorld.hpp"

/*
* Uproszczony model sterowania droną: drona śledzi punkt(pozycja + orientacja)
* Silnik zajmuje się redukcją wszystkich zewnętrzych sił działających na obiekt(z opóźnieniem, żeby było ładniej)
* Osiąganie punktu odbywa się za pomocą sił, jedynie na podstawie różnicy pozycji(potem jeszcze prędkość i przyspieszenie w połączeniu z predykcją trasy)
*/

DroneLikeControl::DroneLikeControl(PhysicalWorld& physics, Vehicle& eq, btVector3 position) :
    VehicleControlSystem(eq),
    m_physics(physics),
    m_virtualPosition(position),
    m_lookDirection(0,1,0)
{
    m_constraint = new btGeneric6DofConstraint(*vehicle.rgBody, btTransform::getIdentity(), true);
    // m_constraint->setAngularLowerLimit(btVector3(-1.0, -1.0, -1.0));
    // m_constraint->setAngularUpperLimit(btVector3(1.0, 1.0, 1.0));
    m_constraint->setLinearLowerLimit(btVector3(-100,-100,-100));
    m_constraint->setLinearUpperLimit(btVector3(100,100,100));

    m_physics.m_dynamicsWorld->addConstraint(m_constraint);
}
DroneLikeControl::~DroneLikeControl(){
    // m_physics.m_dynamicsWorld->removeConstraint(m_constraint);
    // delete m_constraint;
}

void DroneLikeControl::update(float dt){}

void DroneLikeControl::computeState(){
    if(vehicle.control.controlOnAxes.x == 0.f and vehicle.control.controlOnAxes.y == 0.f and vehicle.control.controlOnAxes.z == 0.f){
        if(vehicle.rgBody->getLinearVelocity().length2() > 0.001f) state = Breaking;
        else state = Steady;
    }
    else state = Moving;
}
float DroneLikeControl::accelerationAccordingToState() const {
    switch(state){
        case Steady: return 0.f;
        case Breaking: return 1.f;
        case Moving: return 3.f;
        default: return 0;
    }
}

btVector3 DroneLikeControl::getMoveDirection(glm::vec4 control){
    auto mat = vehicle.btTrans;
    auto forward = mat.getBasis().getColumn(1);
    auto right = mat.getBasis().getColumn(0);
    btVector3 up = btVector3(0,0,1);
    auto out = (forward*control.y + right*control.x + up*control.z).normalized();

    return out;
}

void DroneLikeControl::adjustTargetHeightIfNeeded(btVector3& position){
    if(not m_heightAboveSurface) return;

    // perform raycast and read height
}

void DroneLikeControl::adjustDirection(btVector3& direction){
    auto angle = direction.angle(btVector3(0,0,1));
    if(float limit = toRad*40; abs(angle) > limit){
        direction = direction.rotate(direction.cross(btVector3(0,0,1)), limit - angle);
    }
}

void DroneLikeControl::updateInsidePhysicsStep(float dt){
    computeState();

    auto aimAt = convert(vehicle.control.aimingAt);

    if(m_virtualPosition.distance(aimAt) > 20.f)
        m_virtualDirection = (aimAt - m_virtualPosition).normalized();

    if(state == Moving){
        auto virtualPointMoveDirection = getMoveDirection(vehicle.control.controlOnAxes);
        m_velocity += accelerationAccordingToState()*dt;
        m_virtualPosition += virtualPointMoveDirection * m_velocity*dt;
    }
    adjustTargetHeightIfNeeded(m_virtualPosition);
    adjustDirection(m_virtualDirection);

    btTransform tr;
    vehicle.rgBody->getMotionState()->getWorldTransform(tr);

    positionPart(dt, tr);
    orientationPart(dt, tr);
}

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

void DroneLikeControl::positionPart(float dt, btTransform& tr){
    // cleanup forces
    btVector3 externalForces = vehicle.rgBody->getTotalForce() - m_previouslyappliedForce;
    btVector3 response = -externalForces;
    m_previouslyappliedForce = response;
    // btVector3 response = pdRegForce.goTo(btVector3(0,0,0), externalForces);
    vehicle.rgBody->applyCentralForce(response);

    auto positionError = m_virtualPosition - tr.getOrigin();
    auto impulse = pdRegPosition.goTo(btVector3(0,0,0), -positionError);

    vehicle.rgBody->setLinearVelocity(impulse);
    console.flog("impulse:", impulse,
                 "m_virtualDirection:", m_virtualDirection,
                 "m_velocity:", m_velocity,
                 "m_virtualPosition:", m_virtualPosition,
                 "positionError:", positionError);
    // vehicle.rgBody->applyCentralImpulse(impulse);


}
btVector3 QuaternionToEulerXYZ(const btQuaternion &quat){
    btVector3 euler;
    btScalar w=quat.getW();	btScalar x=quat.getX();	btScalar y=quat.getY();	btScalar z=quat.getZ();
    double sqw = w*w; double sqx = x*x; double sqy = y*y; double sqz = z*z;
    euler.setZ((atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw))));
    euler.setX((atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))));
    euler.setY((asin(-2.0 * (x*z - y*w))));
    return euler;
}
btVector3 QuaternionToEulerXYZ(const glm::quat &quat){
    btVector3 euler;
    btScalar w=quat.w;	btScalar x=quat.x;	btScalar y=quat.y;	btScalar z=quat.z;
    double sqw = w*w; double sqx = x*x; double sqy = y*y; double sqz = z*z;
    euler.setZ((atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw))));
    euler.setX((atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw))));
    euler.setY((asin(-2.0 * (x*z - y*w))));
    return euler;
}

void DroneLikeControl::orientationPart(float dt, btTransform& tr){
    btVector3 currentTorque = vehicle.rgBody->getTotalTorque() - m_previouslyappliedTorque;

    btVector3 response = -currentTorque;
    m_previouslyappliedTorque = response;
    // vehicle.rgBody->applyTorque(response);

    const btVector3 yaxis(0,1,0);
    btVector3 dir(m_virtualDirection[0], m_virtualDirection[1], 0);
    dir.normalize();

    btQuaternion quat(dir.cross(yaxis).normalized(), yaxis.angle(dir));
    float x,y,z;
    quat.getEulerZYX(z,x,y);

    m_constraint->setAngularLowerLimit({x,y,z});
    m_constraint->setAngularUpperLimit({x,y,z});
}
