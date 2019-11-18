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

void Frustum::splitForCSM(u32 parts) {
    std::vector<FrustmCorners> out;
    splitDistances.clear();

    std::vector<float> distances(5);
    float distance = zFar - zNear;
    distances[0] = 0;
    distances[4] = distance;
    float l = 0.8;
    float j = 1;
    auto splitVector = [&, this](glm::vec4 a, glm::vec4 b, float p) {
        return b + p / distance * glm::distance(a, b) * glm::normalize(a - b);
    };
    auto splitFrustum = [&, this](float division, int sliceIndex) {
        slices[sliceIndex].topRight = splitVector(slices[4].slice.TopRight, slices[0].slice.TopRight, division);
        slices[sliceIndex].topLeft = splitVector(slices[4].slice.TopLeft, slices[0].slice.TopLeft, division);
        slices[sliceIndex].bottomRight =
            splitVector(slices[4].slice.BottomRight, slices[0].slice.BottomRight, division);
        slices[sliceIndex].bottomLeft = splitVector(slices[4].slice.BottomLeft, slices[0].slice.BottomLeft, division);
    };

    for(auto i = 1; i < 3; i++, j += 1) {
        auto far = zFar * 0.75f;
        distances[i + 1] = glm::mix(zNear + (j / parts) * (far - zNear), zNear * powf(far / zNear, j / parts), l);
        splitFrustum(distances[i + 1]);
        splitDistances.push_back(distances[i + 1]);
    }

    return out;
}

bool Frustum::intersects(glm::vec4 position, glm::vec2 size) const {
    return true;
}
}