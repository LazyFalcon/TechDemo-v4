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
    // m_constraint = new btGeneric6DofConstraint(*vehicle.rgBody, btTransform::getIdentity(), true);
    // m_constraint->setAngularLowerLimit(btVector3(-1.0, -1.0, -1.0));
    // m_constraint->setAngularUpperLimit(btVector3(1.0, 1.0, 1.0));
    // m_constraint->setLinearLowerLimit(btVector3(-100,-100,-100));
    // m_constraint->setLinearUpperLimit(btVector3(100,100,100));

    // m_physics.m_dynamicsWorld->addConstraint(m_constraint);
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

btVector3 DroneLikeControl::getMoveDirection(glm::vec4 control) const {
    const auto& mat = vehicle.btTrans;
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

float angleToPlane(const btVector3& planeNormal, const btVector3& vector){
    return (pi/2 - planeNormal.angle(vector));
}

void DroneLikeControl::adjustDirection(btVector3& direction){
    auto inclination = angleToPlane(btVector3(0,0,1), direction);
    if(float maxInclination = toRad*40; abs(inclination) > maxInclination){
        direction = direction.rotate(direction.cross(btVector3(0,0,1)), maxInclination - inclination);
    }
}

void DroneLikeControl::updateInsidePhysicsStep(float dt){
    computeState();

    auto aimAt = convert(vehicle.control.aimingAt);

    // todo: aimAt nie powieni sięzmieniać ..
    m_virtualDirection = (aimAt - m_virtualPosition).normalized();
    // m_virtualDirection = btVector3(1,1,1).normalized();

    if(state == Moving){
        auto virtualPointMoveDirection = getMoveDirection(vehicle.control.controlOnAxes);
        m_velocity += accelerationAccordingToState()*dt;
        m_virtualPosition += virtualPointMoveDirection * m_velocity*dt;
    }
    adjustTargetHeightIfNeeded(m_virtualPosition);
    // adjustDirection(m_virtualDirection);

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
    vehicle.rgBody->setLinearFactor({0,0,0});
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

btVector3 DroneLikeControl::getDesiredAngles(const btVector3& targetDir, const btVector3& normalToPlane, const btVector3& rightAxis){
    return btVector3(
        angleToPlane(normalToPlane, targetDir),
        angleToPlane(normalToPlane, rightAxis),
        -atan2(targetDir[0], targetDir[1])
    );
}

void DroneLikeControl::orientationPart(float dt, const btTransform& tr){

    const auto worldAngularVelocity = vehicle.rgBody->getAngularVelocity();
    const auto localAngularVelocity = tr.getBasis().solve33(worldAngularVelocity);
    const auto directionInLocalSpace = tr.getBasis().solve33(m_virtualDirection);
    const auto locX = tr.getBasis().getColumn(0);
    const auto locY = tr.getBasis().getColumn(1);
    const auto locZ = tr.getBasis().getColumn(2);
    const btVector3 targetLocalAngles(
        angleToPlane(locZ, m_virtualDirection),
        angleToPlane(btVector3(0,0,1), locX),
        -atan2(directionInLocalSpace[0], directionInLocalSpace[1])
    );

    btVector3 angularVelocityToApply(0,0,0);

    // console.log(targetLocalAngles, directionInLocalSpace, localAngularVelocity);
    //  [ 0.61548, -0, -0.785398 ] [ 0.57735, 0.57735, 0.57735 ] [ 0, 0, 0 ]
    { // control inclination, allow to small move with target direction - best is to limit it directly in target
        const float maxSpeed = pi/2; // full rotation in one second, needs to be quick
        const float maxVelocityChange = maxSpeed * 60.f/2.f; // reach max speed in 3 frames maxSpeed/
        { // for X axis
            const auto orientedAngle = targetLocalAngles[0];
            const float desiredSpeed = glm::sign(orientedAngle) * glm::smoothstep(0.f, pi/30, abs(orientedAngle))*maxSpeed;
            const auto speedDelta = desiredSpeed - localAngularVelocity[0];
            const auto speedChange = glm::sign(speedDelta) * std::min(abs(speedDelta), maxVelocityChange);

            if(abs(orientedAngle) > 0.001f)
                angularVelocityToApply += locX*(localAngularVelocity[0]+speedChange);
        }
        { // for Y axis
            const auto orientedAngle = targetLocalAngles[1];
            const float desiredSpeed = glm::sign(orientedAngle) * glm::smoothstep(0.f, pi/30, abs(orientedAngle))*maxSpeed;
            const auto speedDelta = desiredSpeed - localAngularVelocity[1];
            const auto speedChange = glm::sign(speedDelta) * std::min(abs(speedDelta), maxVelocityChange);

            if(abs(orientedAngle) > 0.001f)
                angularVelocityToApply += locY*(localAngularVelocity[1]+speedChange);
        }
    }
    { // control velocites directly, but consider previous velocity, so there is a cap for applied velocity change
        const float maxSpeed = pi/3; // half of rotation in one second
        const float maxVelocityChange = maxSpeed * 60.f/3.f; // reach max speed in 3 frames maxSpeed/
        const auto orientedAngle = targetLocalAngles[2];
        // const auto orientedAngle = dir.angle(locY) * ((locY.cross(dir).dot(locZ) > 0.f) ? 1.f : -1.f);
        const float desiredSpeed = glm::sign(orientedAngle) * glm::smoothstep(0.f, pi/20, abs(orientedAngle))*maxSpeed; // smooth speed near target angle

        const auto speedDelta = desiredSpeed - localAngularVelocity[2];
        const auto speedChange = glm::sign(speedDelta) * std::min(abs(speedDelta), maxVelocityChange);

        if(abs(orientedAngle) > 0.001f)
            angularVelocityToApply += locZ*(localAngularVelocity[2]+speedChange);
        // else try to zero velocity
    }
    vehicle.rgBody->setAngularVelocity(angularVelocityToApply);
}
