#include "core.hpp"
#include "camera-frustum.hpp"

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

    std::vector<float> distances(5);
    float totalDistance = zFar - zNear;
    float l = 0.8;
    float j = 1;
    auto splitVector = [&, this](glm::vec4 a, glm::vec4 b, float p) {
        return b + p / totalDistance * glm::distance(a, b) * glm::normalize(a - b);
    };
    auto splitFrustum = [&, this](float division, int sliceIndex) {
        slices.slice[sliceIndex].topRight = splitVector(slices.slice[4].topRight, slices.slice[0].topRight, division);
        slices.slice[sliceIndex].topLeft = splitVector(slices.slice[4].topLeft, slices.slice[0].topLeft, division);
        slices.slice[sliceIndex].bottomRight =
            splitVector(slices.slice[4].bottomRight, slices.slice[0].bottomRight, division);
        slices.slice[sliceIndex].bottomLeft =
            splitVector(slices.slice[4].bottomLeft, slices.slice[0].bottomLeft, division);
    };

    for(auto i = 1; i < 3; i++, j += 1) {
        auto far = zFar * 0.75f;
        auto distance = glm::mix(zNear + (j / parts) * (far - zNear), zNear * powf(far / zNear, j / parts), l);
        splitFrustum(distance, i);
        splitDistances.push_back(distance);
    }
}

bool Frustum::intersects(glm::vec4 position, glm::vec2 size) const {
    return true;
}
}
