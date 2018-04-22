#include "Includes.hpp"
#include "Frustum.hpp"

bool Frustum::testSphere(glm::vec4 position, float radius) const {
    for(const auto &plane : planes.array)
        if(glm::dot(plane, position) > radius)
            return false;
    return true;
}
bool Frustum::testAABB(glm::vec4 box) const {
    return true;
}

std::vector<FrustmCorners> Frustum::splitForCSM(u32 parts){
    std::vector<FrustmCorners> out;
    splitDistances.clear();

    std::vector<float> distances(parts+1);
    distances[0] = 0;
    float l = 0.8;
    float j = 1;
    float distance = zFar - zNear;
    auto splitVector = [&, this](glm::vec4 a, glm::vec4 b, float p){
        return b + p/distance * glm::distance(a, b)*glm::normalize(a-b);
    };
    auto splitFrustum = [&, this](float start, float end){
        out.push_back(corners);
        auto &corner = out.back();
        corner.m.farTopRight = splitVector(corner.m.farTopRight, corner.m.nearTopRight, end);
        corner.m.farTopLeft = splitVector(corner.m.farTopLeft, corner.m.nearTopLeft, end);
        corner.m.farBottomRight = splitVector(corner.m.farBottomRight, corner.m.nearBottomRight, end);
        corner.m.farBottomLeft = splitVector(corner.m.farBottomLeft, corner.m.nearBottomLeft, end);
        corner.m.farCenter = splitVector(corner.m.farCenter, corner.m.nearCenter, end);

        corner.m.nearTopRight = splitVector(corner.m.farTopRight, corner.m.nearTopRight, start);
        corner.m.nearTopLeft = splitVector(corner.m.farTopLeft, corner.m.nearTopLeft, start);
        corner.m.nearBottomRight = splitVector(corner.m.farBottomRight, corner.m.nearBottomRight, start);
        corner.m.nearBottomLeft = splitVector(corner.m.farBottomLeft, corner.m.nearBottomLeft, start);
        corner.m.nearCenter = splitVector(corner.m.farCenter, corner.m.nearCenter, start);
    };

    for(auto i=0; i<parts; i++, j+=1){
        auto far = zFar * 0.75f;
        distances[i+1] = glm::mix(
            zNear + (j/parts) * (far - zNear),
            zNear*powf(far/zNear, j/parts),
            l);
        splitFrustum(distances[i], distances[i+1]);
        splitDistances.push_back(distances[i+1]);
    }

    return out;
}

bool Frustum::intersects(glm::vec4 position, glm::vec2 size) const {

    return true;
}
