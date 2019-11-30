#include "core.hpp"
#include "camera-frustum.hpp"
#include "Logger.hpp"

namespace camera
{
bool Frustum::testSphere(glm::vec4 position, float radius) const {
    for(const auto& plane : planes.array)
        if(glm::dot(plane, position) > radius)
            return false;
    return true;
}
bool Frustum::testAABB(glm::vec4 box) const {
    return true;
}

void Frustum::splitForCSM(int parts) {
    splitDistances.clear();

    std::vector<float> distances {0.038f, 0.08f, 0.2f, 1.f};
    float totalDistance = zFar - zNear;
    float l = 0.8;
    float j = 1;
    auto splitVector = [&, this](glm::vec4 far, glm::vec4 near, float split) {
        return near + split * glm::distance(far, near) * glm::normalize(far - near);
    };
    auto splitFrustum = [&, this](float division, int sliceIndex) {
        // todo: later: glm::mix(near, far, split);
        slices.slice[sliceIndex].topRight = splitVector(slices.slice[4].topRight, slices.slice[0].topRight, division);
        slices.slice[sliceIndex].topLeft = splitVector(slices.slice[4].topLeft, slices.slice[0].topLeft, division);
        slices.slice[sliceIndex].bottomRight =
            splitVector(slices.slice[4].bottomRight, slices.slice[0].bottomRight, division);
        slices.slice[sliceIndex].bottomLeft =
            splitVector(slices.slice[4].bottomLeft, slices.slice[0].bottomLeft, division);
    };

    for(auto i = 0; i < 3; i++, j += 1) {
        // const auto far = zFar * 0.75f;
        // auto distance = glm::mix(zNear + (j / parts) * (far - zNear), zNear * powf(far / zNear, j / parts), l);
        splitFrustum(distances[i], i + 1);
        splitDistances.push_back(totalDistance * distances[i]);
    }
    splitDistances.push_back(totalDistance * distances[3]);
}

bool Frustum::intersects(glm::vec4 position, glm::vec2 size) const {
    return true;
}
}
