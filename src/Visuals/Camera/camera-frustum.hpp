#pragma once
#include <array>

namespace camera
{
// todo: why not define only far corners? and get near corners from previous?

struct Slice
{
    glm::vec4 topRight;
    glm::vec4 topLeft;
    glm::vec4 bottomRight;
    glm::vec4 bottomLeft;
};

union FrustmCorners
{
    std::array<Slice, 5> slice;
    std::array<glm::vec4, 4 * 5> point;
};

union FrustumPlanes
{
    struct u
    {
        glm::vec4 topPlane;
        glm::vec4 bottomPlane;
        glm::vec4 rightPlane;
        glm::vec4 leftPlane;
        glm::vec4 nearPlane;
        glm::vec4 farPlane;
    } m;
    std::array<glm::vec4, 6> array {};
};

union FrustumVectors
{
    struct u
    {
        glm::vec4 right;
        glm::vec4 up;
        glm::vec4 forward;
    } m;
    std::array<glm::vec4, 3> array {};
};

/**
 *  normalne są skierowane na zewnątrz,
 *  kolejnośc punktów ma znaczenie?
 *  odleglosc jest miezona w metryce manhattan
 *  przenieć obliczanie odleglosci
 *  test przesaniania również powinien si znaleźć wyżej
 */
class Frustum
{
public:
    bool testSphere(glm::vec4 position, float radius) const;
    bool testAABB(glm::vec4 box) const;
    bool intersects(glm::vec4 position, glm::vec2 size) const;
    std::vector<FrustmCorners> splitForCSM(u32 parts = 4);
    std::vector<float> splitDistances;

    FrustmCorners slices {};
    FrustumPlanes planes {};
    std::array<glm::vec4, 5> cornerVectors;
    FrustumVectors vectors {};
    glm::vec4 eye;
    float fov;
    float aspect;
    float zNear, zFar;
};
}
