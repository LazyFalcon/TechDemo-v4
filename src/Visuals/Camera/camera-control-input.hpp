#pragma once

namespace camera
{

// ? jeśli controller będzie jeden(ze stanami), to czemu by nie włożyć tego do controllera?
struct ControlInput
{
    glm::vec4 targetPosition;
    glm::vec2 relativePointerMove;
    std::optional<glm::vec4> setDirection;
    std::optional<glm::vec4> setTargetPosition;

    enum class FreeCamMode {Around, InPlace} mode {FreeCamMode::Around};
    glm::vec3 move;
    glm::vec4 velocity;


};

}
