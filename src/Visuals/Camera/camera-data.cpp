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

    // frustum.slices[0].slice.center = nearCenter;
    // frustum.slices[4].slice.center = farCenter;
    frustum.slices[4].slice.topRight = farCenter + up * farHeight + right * farWidth;
    frustum.slices[4].slice.topLeft = farCenter + up * farHeight - right * farHeight;
    frustum.slices[4].slice.bottomRight = farCenter - up * farHeight + right * farWidth;
    frustum.slices[4].slice.bottomLeft = farCenter - up * farHeight - right * farWidth;

    farWidth = tan(fov * 0.5f) * nearDistance;
    farHeight = farWidth / aspectRatio;
    frustum.slices[0].slice.topRight = nearCenter + up * farHeight + right * farWidth;
    frustum.slices[0].slice.topLeft = nearCenter + up * farHeight - right * farWidth;
    frustum.slices[0].slice.bottomRight = nearCenter - up * farHeight + right * farWidth;
    frustum.slices[0].slice.bottomLeft = nearCenter - up * farHeight - right * farWidth;

    frustum.planes.m.topPlane =
        plane(frustum.slices[4].slice.topRight, frustum.slices[4].slice.topLeft, orientation[3]);
    frustum.planes.m.bottomPlane =
        plane(frustum.slices[4].slice.bottomLeft, frustum.slices[4].slice.bottomRight, orientation[3]);
    frustum.planes.m.leftPlane =
        plane(frustum.slices[4].slice.topLeft, frustum.slices[4].slice.bottomLeft, orientation[3]);
    frustum.planes.m.rightPlane =
        plane(frustum.slices[4].slice.bottomRight, frustum.slices[4].slice.topRight, orientation[3]);
    frustum.planes.m.nearPlane = nearPlane;
    frustum.planes.m.farPlane = farPlane;

    frustum.vectors.m.forward = at;
    frustum.vectors.m.right = right;
    frustum.vectors.m.up = up;

    frustum.cornerVectors = {glm::normalize(frustum.slices[4].slice.BottomLeft - orientation[3]),
                             glm::normalize(frustum.slices[4].slice.TopLeft - orientation[3]),
                             glm::normalize(frustum.slices[4].slice.BottomRight - orientation[3]),
                             glm::normalize(frustum.slices[4].slice.TopRight - orientation[3]), at};
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
