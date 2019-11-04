#include "core.hpp"
#include "camera-data.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

namespace camera
{
void Camera::recalculate() {
    view = glm::affineInverse(orientation);

    right = orientation[0];
    up = orientation[1];
    at = -orientation[2];

    PV = projection * view;
    invPV = glm::inverse(PV);

    recalucuateFrustum();
}

void Camera::changeFov(float dy) {
    fov -= 10.5 * dy * toRad;
    fov = glm::clamp(fov, 1.5f * toRad, 150.f * toRad);
    recalucuateProjectionMatrix();
}

void Camera::recalucuateProjectionMatrix() {
    float near = (abs(offset.z) < 1.f) ? 0.1 : nearDistance;

    projection = glm::perspective(fov / aspectRatio, aspectRatio, near, farDistance);
    projectionInverse = glm::inverse(projection);
}

void Camera::changeOffset(float x, float y, float z) {
    offset += glm::vec4(x, y, z, 0);
}
// todo: move to frustum
void Camera::recalucuateFrustum() {
    frustum.eye = orientation[3];
    // nie powinny byc *0.5?
    float farWidth = tan(fov * 0.5f) * farDistance * 1.3;
    float farHeight = farWidth / aspectRatio;
    glm::vec4 farCenter = at * farDistance + orientation[3];
    glm::vec4 nearCenter = at * nearDistance + orientation[3];

    glm::vec4 farPlane = plane(at, farCenter);
    glm::vec4 nearPlane = plane(-at, nearCenter);

    frustum.fov = fov;
    frustum.aspect = aspectRatio;
    frustum.zNear = nearDistance;
    frustum.zFar = farDistance;

    frustum.corners.m.nearCenter = nearCenter;
    frustum.corners.m.farCenter = farCenter;
    frustum.corners.m.farTopRight = farCenter + up * farHeight + right * farWidth;
    frustum.corners.m.farTopLeft = farCenter + up * farHeight - right * farHeight;
    frustum.corners.m.farBottomRight = farCenter - up * farHeight + right * farWidth;
    frustum.corners.m.farBottomLeft = farCenter - up * farHeight - right * farWidth;

    farWidth = tan(fov * 0.5f) * nearDistance;
    farHeight = farWidth / aspectRatio;
    frustum.corners.m.nearTopRight = nearCenter + up * farHeight + right * farWidth;
    frustum.corners.m.nearTopLeft = nearCenter + up * farHeight - right * farWidth;
    frustum.corners.m.nearBottomRight = nearCenter - up * farHeight + right * farWidth;
    frustum.corners.m.nearBottomLeft = nearCenter - up * farHeight - right * farWidth;

    frustum.planes.m.topPlane = plane(frustum.corners.m.farTopRight, frustum.corners.m.farTopLeft, orientation[3]);
    frustum.planes.m.bottomPlane =
        plane(frustum.corners.m.farBottomLeft, frustum.corners.m.farBottomRight, orientation[3]);
    frustum.planes.m.leftPlane = plane(frustum.corners.m.farTopLeft, frustum.corners.m.farBottomLeft, orientation[3]);
    frustum.planes.m.rightPlane =
        plane(frustum.corners.m.farBottomRight, frustum.corners.m.farTopRight, orientation[3]);
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

    frustum.cornerVectors = {glm::normalize(frustum.corners.m.farBottomLeft - orientation[3]),
                             glm::normalize(frustum.corners.m.farTopLeft - orientation[3]),
                             glm::normalize(frustum.corners.m.farBottomRight - orientation[3]),
                             glm::normalize(frustum.corners.m.farTopRight - orientation[3]), at};
}

float Camera::convertDepthToWorld(float depth) {
    depth = 2 * depth - 1;
    depth = (2 * nearDistance * farDistance) / (nearDistance + farDistance - depth * (farDistance - nearDistance));
    return depth;
}

void Camera::printDebug() {
    console.log("fov:", fov * toDeg, "\taspect:", aspectRatio);
    console.log("distances:", nearDistance, "\tfarDistance:", farDistance);
    console.log("matrix:", orientation);
    console.log("offset:", offset, offsetScale);
}
}
