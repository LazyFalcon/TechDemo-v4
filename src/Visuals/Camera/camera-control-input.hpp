#pragma once

namespace camera
{
/*

Jak rozwiązać input? w playground/player jest to zrobione na eventach, więc co ramkę nie wszytko będzie wypełniane.

Z drugiej strony różne stany kamery potrzebują różnego inputu, myslę że nie trzeba by user jakoś to różnicował i niech wypełnia wszysto

*/
struct ControlInput
{
    glm::vec4 targetPosition;
    glm::vec2 relativePointerMove;
    std::optional<glm::vec4> setDirection;
    std::optional<glm::vec4> setTargetPosition;

    enum class FreeCamMode {Around, InPlace} mode {FreeCamMode::Around};
    glm::vec3 move;
    glm::vec4 velocity;

    glm::vec4 pointerPosition;
    std::optional<glm::vec4> worldPositionToRotateAround;

    // wypełniane przez procedury controllera
    struct {
        bool keepPointerInPosition;
    } feedback;

    // po użyciu czyszczone przez kontroller
    struct {
        float zoomToMouse;

    } once;
};

}
