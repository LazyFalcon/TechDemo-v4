#pragma once

constexpr float pi = 3.141592f;
constexpr double dpi = 3.1415926535;
constexpr double dpi2 = 2 * 3.1415926535;
constexpr float toRad = pi / 180.f;
constexpr float toGrad = 1.f / (pi * 2.f);
constexpr float fromGrad = pi * 2.f;
constexpr float fromRad = 180.f / pi;
constexpr float toDeg = 180.f / pi;
constexpr float hpi = pi * 0.5f;
constexpr float pi2 = pi * 2.f;

constexpr float toMS = 5.f / 18.f;
constexpr float toKMH = 18.f / 5.f;

const glm::vec4 X(1, 0, 0, 0);
const glm::vec4 Y(0, 1, 0, 0);
const glm::vec4 Z(0, 0, 1, 0);
const glm::vec4 W(0, 0, 0, 1);
const glm::vec4 X4(1, 0, 0, 0);
const glm::vec4 Y4(0, 1, 0, 0);
const glm::vec4 Z4(0, 0, 1, 0);
const glm::vec4 W4(0, 0, 0, 1);

const glm::vec3 X3(1, 0, 0);
const glm::vec3 Y3(0, 1, 0);
const glm::vec3 Z3(0, 0, 1);

const glm::mat4 identityMatrix(1);
const btTransform btIdentity = btTransform::getIdentity();
const glm::quat qIdentity = glm::quat(0, 0, 0, 1);

constexpr float frameMs = 1.f / 60.f * 1000.f;
