#pragma once

namespace camera
{
/*

Jak rozwiązać input? w playground/player jest to zrobione na eventach, więc co ramkę nie wszytko będzie wypełniane.

Z drugiej strony różne stany kamery potrzebują różnego inputu, myslę że nie trzeba by user jakoś to różnicował i niech wypełnia wszysto

*/
struct ControlInput
{
    bool keepRightAxisHorizontal = false; // target right vector always in horizontal plane so roll will be zero
    bool parentRotationAffectCurrentRotation = false; // parent rotation messes with current rotation
    bool smoothParentRotation = false; // slerp of current rotation with parent rotation

    std::optional<glm::vec4> worldPointToFocusOn; // artifical input or updated when moving camera
    glm::vec4 directionToAlignCamera; // applies to target rotation
    bool directionIsInLocalSpace = false; // multiply target by parent rotation

    // powinno być gdzieś indziej
    bool whenNotMovingFocusOnPoint = false;
    std::optional<glm::vec4> worldPointToFocusOnWhenNotMoving; // updated when moving camera














    // może zbierzmy co jest potrzeben dla każdego modułu?

    // freecam:
    glm::vec3 directionOfMovement
    bool switchToMovementOnWorldAxes {false};
    std::optional<glm::vec4> rotateAroundThisPoint;
    struct {
        float vertical;
        float horizontal;
        float roll;
    } pointerMovement;
    bool reqiuresToHavePointerInTheSamePosition;






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
