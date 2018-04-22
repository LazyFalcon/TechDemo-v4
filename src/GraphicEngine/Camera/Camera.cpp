#include "Utils.hpp"
#include "Includes.hpp"
#include "Camera.hpp"
#include "Logging.hpp"

void Exposure::update(float &exposure, float dt){
    exposure = 5;
    // exposure = glm::mix(exposure, target, acomodation*dt/16.f*0.02f);
}
// TODO: use some kind of curve?
void Camera::calculateExposure(float luminance){
    if(std::isnan(luminance)) exposureEvaluator.target = 10;
    else exposureEvaluator.target = glm::clamp(10.f/luminance, 0.1f, 30.f);
}

void Camera::update(float dt){
    constraints.update(*this, dt);
    target.update(*this, dt);
    exposureEvaluator.update(exposure, dt);
}

void Camera::calc(float dt){
    update(dt);
    position.w = 1.f;

    origin.x = glm::clamp(origin.x, positionBounds[0], positionBounds[1]);
    origin.y = glm::clamp(origin.y, positionBounds[2], positionBounds[3]);
    position = origin + postOffset + glm::vec4(0, offset.y*(1.f + distanceToOrigin/25.f), distanceToOrigin, 0)*basis;
    position.x = glm::clamp(position.x, positionBounds[0], positionBounds[1]);
    position.y = glm::clamp(position.y, positionBounds[2], positionBounds[3]);

    if(cropCameraHeight){
         auto newZ = std::max(position.z, referenceHeihgt + 1.f);
         origin.z += newZ - position.z;
         position.z = newZ;
    }

    view = glm::toMat4(basis) * glm::translate(-position.xyz());
    at = (-Z) * view;
    right =  X * view;
    up = Y * view;
    at.w = 0;
    right.w = 0;
    up.w = 0;
    glm::normalize(right);
    glm::normalize(up);
    glm::normalize(at);

    projectionInv = glm::inverse(projection);
    viewInv = glm::inverse(view);
    PV = projection*view;
    invPV = glm::inverse(PV);

    calcFrustum();
}

void CameraConstraints::update(Camera &camera, float dt){
    if(not use)
        return;

    camera.fov = glm::clamp(camera.fov, fov.x, fov.y);
    camera.distanceToOrigin = glm::clamp(camera.distanceToOrigin, distance.x, distance.y);
    camera.euler.x = period(camera.euler.x, horizontal.x, horizontal.x);
    camera.euler.y = glm::clamp(camera.euler.y, vertical.x, vertical.y);

}

void CameraTargetEvaluator::update(Camera &camera, float dt){
    if(use){
        camera.basis = glm::slerp(camera.basis, basis, basisSmooth*dt/16.f);
        camera.origin = glm::mix(camera.origin, origin, positionSmooth*dt/16.f);
        // camera.origin = origin;
    }
    else {
        camera.basis = basis;
    }
}
void Camera::updateSmootPosition(glm::vec4 newOrigin){
    target.origin = newOrigin;

    // center of rotation is on horizontal circle, radius 4
    // auto vec = glm::vec4(0,-1,1,0)*target.basis;
    // vec.z = 0;
    // vec = glm::normalize(vec);
    // vec = postOffset + glm::vec4(0,0,distanceToOrigin,0)*target.basis - vec*4.f;
    // target.position = newOrigin + offset + vec;
}

void Camera::setOrigin(glm::vec4 o){
    origin = o;
}
void Camera::setPosition(glm::vec4 o){
    updateSmootPosition(o);
}


void Camera::moveOriginTo(glm::vec4 o){
    auto delta = origin - o;
    origin = o;
    // position += delta;
    target.position += delta;
}

glm::vec4 Camera::directlyMove(glm::vec4 cameraSpaceVelocities, float dt){
    cameraSpaceVelocities = basis * cameraSpaceVelocities;

    target.position += cameraSpaceVelocities * dt;

    return cameraSpaceVelocities;
}

glm::vec4 Camera::directlyMoveConstZ(glm::vec4 cameraSpaceVelocities, float z,  float dt){
    cameraSpaceVelocities = cameraSpaceVelocities * basis;

    auto oz = origin.z;
    target.origin -= cameraSpaceVelocities * dt;
    // origin.z = oz;
    // target.position -= cameraSpaceVelocities * dt;
    // updateSmootPosition(origin - cameraSpaceVelocities * dt);
    // updateSmootPosition(origin);

    return cameraSpaceVelocities;
}

void Camera::cutZ(glm::vec4 p){
    position.z = std::max(position.z,  p.z+1.f);
    target.position.z = std::max(target.position.z,  p.z+0.3f);
}

void Camera::rotate(glm::vec2 vp){
    glm::vec2 v(
        vp.x*cos(euler.z) - vp.y*sin(euler.z),
        vp.x*sin(euler.z) + vp.y*cos(euler.z));

    euler.x += (v.x * 12.f * fov)/pi;
    euler.y += (v.y * 12.f * fov)/pi;
    target.basis = glm::angleAxis(euler.y, X3) *  glm::angleAxis(euler.x, Z3);
    // target.basis = glm::angleAxis(euler.y, (X4*target.basis).xyz()) *  glm::angleAxis(euler.x, (Z4*target.basis).xyz());
    // updateSmootPosition(origin); // uncoment if something strange will happen, but then freecam will stop moving
}

void Camera::zoomDistance(float dy){
    distanceToOrigin -= 0.5 * dy;
    calcProjection();
}

void Camera::zoomFov(float dy){
    fov -= 10.5 * dy * toRad;
    fov = glm::clamp(fov, 1.5f * toRad, 150.f * toRad);
    calcProjection();
}

void Camera::calcProjection(){
    if(abs(distanceToOrigin) < 1.f) nearDistance = 0.1;
    else nearDistance = 1;
    nearDistance = 1;
    farDistance = 2000;

    projection = glm::perspective(fov/aspect, aspect, nearDistance, farDistance);
}

void Camera::calcFrustum(){
    frustum.eye = position;
     // nie powinny byc *0.5?
    farWidth = tan(fov*0.5f)*farDistance*1.3;
    farHeight = farWidth/aspect;
    glm::vec4 farCenter = at*farDistance + position;
    glm::vec4 nearCenter = at*nearDistance + position;

    glm::vec4 farPlane = plane(at, farCenter);
    glm::vec4 nearPlane = plane(-at, nearCenter);

    frustum.fov = fov;
    frustum.aspect = aspect;
    frustum.zNear = nearDistance;
    frustum.zFar = farDistance;

    frustum.corners.m.nearCenter = nearCenter;
    frustum.corners.m.farCenter = farCenter;
    frustum.corners.m.farTopRight = farCenter + up*farHeight + right*farWidth;
    frustum.corners.m.farTopLeft = farCenter + up*farHeight - right*farHeight;
    frustum.corners.m.farBottomRight = farCenter - up*farHeight + right*farWidth;
     frustum.corners.m.farBottomLeft = farCenter - up*farHeight - right*farWidth;


    farWidth = tan(fov*0.5f)*nearDistance;
    farHeight = farWidth/aspect;
    frustum.corners.m.nearTopRight = nearCenter + up*farHeight + right*farWidth;
    frustum.corners.m.nearTopLeft = nearCenter + up*farHeight - right*farWidth;
    frustum.corners.m.nearBottomRight = nearCenter - up*farHeight + right*farWidth;
    frustum.corners.m.nearBottomLeft = nearCenter - up*farHeight - right*farWidth;

    frustum.planes.m.topPlane = plane(frustum.corners.m.farTopRight, frustum.corners.m.farTopLeft, position);
    frustum.planes.m.bottomPlane = plane(frustum.corners.m.farBottomLeft, frustum.corners.m.farBottomRight, position);
    frustum.planes.m.leftPlane = plane(frustum.corners.m.farTopLeft, frustum.corners.m.farBottomLeft, position);
    frustum.planes.m.rightPlane = plane(frustum.corners.m.farBottomRight, frustum.corners.m.farTopRight, position);
    frustum.planes.m.nearPlane = nearPlane;
    frustum.planes.m.farPlane = farPlane;

    // frustum.planes.m.rightPlane = glm::vec4(PV[3]-PV[0], PV[7]-PV[4], PV[11]-PV[8], PV[15]-PV[12]);
    // // Extract the LEFT clipping plane
    // frustum.planes.m.leftPlane = glm::vec4(PV[3]+PV[0], PV[7]+PV[4], PV[11]+PV[8], PV[15]+PV[12]);
    // // Extract the TOP clipping plane
    // frustum.planes.m.topPlane = glm::vec4(PV[3]-PV[1], PV[7]-PV[5], PV[11]-PV[9], PV[15]-PV[13]);
    // // Extract the BOTTOM clipping plane
    // frustum.planes.m.bottomPlane = glm::vec4(PV[3]+PV[1], PV[7]+PV[5], PV[11]+PV[9], PV[15]+PV[13]);
    // // Extract the FAR clipping plane
    // frustum.planes.m.farPlane = glm::vec4(PV[3]-PV[2], PV[7]-PV[6], PV[11]-PV[10], PV[15]-PV[14]);

    frustum.vectors.m.forward = at;
    frustum.vectors.m.right = right;
    frustum.vectors.m.up = up;

    frustum.cornerVectors = {
        glm::normalize(frustum.corners.m.farBottomLeft - position),
        glm::normalize(frustum.corners.m.farTopLeft - position),
        glm::normalize(frustum.corners.m.farBottomRight - position),
        glm::normalize(frustum.corners.m.farTopRight - position),
        at
    };
}

void Camera::showDebug() const {

}

float Camera::convertDepthToWorld(float depth){
    depth = 2*depth - 1;
    depth = (2 * nearDistance * farDistance) / (nearDistance + farDistance - depth * (farDistance - nearDistance));
    return depth;
}
