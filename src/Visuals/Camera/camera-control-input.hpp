#pragma once

namespace camera
{
/*
30.09
-> rozpisać jaki jest input kamery, co jest zbierane i co ma wpływ na kamerę w pojedynczej ramce
    -> zmiana pozycji i obrotu obiektu do którego jest przypięta

Przy zablokowanym wskaźniku input myszy przekłada się na odchylenie kamery
Przy swobodnym wskaźniku input myszy przekłada się na odchylenie kamery, a pozycja wskaźnika na ekranie jest interpretacja odchylenia

Orientajcja kamery jest, o ile pamiętam, wyliczna jako orientajcja globalna

Przy kamerze lokalnej, obrót pojazdu wpływa na obrót kamery(dodawana jest różnica od poprzedniej ramki), oraz na docelową pozycję(pozycja docelowa jest stała w przestrzeni pojazdu)

Blokowanie kamery na punkcie jest intencjonalne, i niezależne od trybu kamery(tak?) wtedy docelowa orientacja kamery w świecie jest wyliczana z pozycji kamery i punktu w który patrzy. Trzeba jakoś uwzględnić to że kamera obraca się na uwięzi. TODO: rozrysować to.

1.10
Z perspektywy czasu, jak to wygląda?

Sterowanie orientacją:
- ruchy myszy zamienianie na kąty eulera
- punkt docelowy w który kamera ma się patrzeć

Tryby pracy:
- zadana orientacja jest podana względem świata
- orientacja jest zadana względem pojazdu
- punkt jest zadany względem świata

Pozycji:
- przypięty do obiektu: target wyciągamy z macieży ownera
- swobodny, dostajemy input i modyfikujemy target

Stabilizacja:
- wyrównujemy kamerę tak by utrzymywała poziom, obroty wokół osi patrzenia

Kopiowanie ruchu i transformacji rodzica:
- całość transformacji wpływa na docelową orientację
- jedynie obroty w poziomie, dookoła globalnej osi Z
- nasladujemy płaszczyznę poziomą
- kopiujemy obroty rodzica do aktualnej orientacji kamery

Wizualizacja wskaźnika:
- swobodny, niezależny od kamery, do trybu blenderowego,
- odchylany, wizualizuje odchylenie kamery, tak że puszczony swobodnie wyrówna się z kamerą i trafi na środek
- przypięty na stałe na środku

Czynności robione poza kamerą, przez usera
- blokowanie punktu przy ADSie
- blokowanie się na punkcie gdy pojazd się nie porusza, lub gdy nie obracamy kamerą
- dynamiczne przełączanie pomiędzy globalnym a lokalnym kierunkiem, po to żeby mieć jakąś ciekawszą stabilizację

- przełączanie pomiędzy trybami wymagać będzie przeliczenia eulera, bo przełączenia muszą być gładkie

*/

struct ControlInput
{
    // control input
    std::optional<glm::vec4> worldPointToFocusOn; // artifical input or updated when moving camera
    std::optional<glm::vec4> worldPointToFocusOnWhenSteady; // updated when moving camera
    std::optional<glm::vec4> worldPointToZoom; // artifical input or updated when zooming
    std::optional<glm::vec4> rotateAroundThisPoint; // for freecam rotations
    std::optional<glm::vec4> directionToAlignCamera; // forces camera to look in that direction
    float zoomDirection = 0;
    glm::vec4 directionOfMovement {};
    struct {
        float horizontal {};
        float vertical {};
        float roll {};
    } pointerMovement;

    // controller behaviour configuration
    bool zoomByFov = false; // otherwise zoom by scalling offset

    bool keepRightAxisHorizontal = false; // target right vector always in horizontal plane so roll will be zero
    bool parentRotationAffectCurrentRotation = false; // parent rotation messes with current rotation
    bool smoothParentRotation = false; // slerp of current rotation with parent rotation
    bool inSteadyFocusOnPoint = false;
    bool directionIsInLocalSpace = false; // multiply target by parent rotation

    bool switchToMovementOnWorldAxes {false};
    bool moveHorizontally {false};

    // feedback to user
    bool reqiuresToHavePointerInTheSamePosition = false;

    // other informations
    bool isPointerMovingFree = true; // powinien zablokować local direction
    bool isParrentFollowingPointer = false;
    bool freecam = false;

    void resetAfterUse(){
        zoomDirection = 0;
        pointerMovement.horizontal = 0;
        pointerMovement.vertical = 0;
        pointerMovement.roll = 0;
        directionOfMovement = {};
    }

    void freeControl(){
        resetAfterUse();
        worldPointToFocusOn.reset();
        worldPointToFocusOnWhenSteady.reset();
        worldPointToZoom.reset();
        rotateAroundThisPoint.reset();
        directionToAlignCamera.reset();
    }
};
/*
Akcje jakie z tego wynikają:
    worldPointToFocusOn - kamera obraca się w kierunku tego punktu,
    worldPointToFocusOnWhenSteady - też, inSteadyFocusOnPoint - to jest dla usera
    worldPointToZoom do tego punktu zoomujemy, jeśli nie ma to do środka
    rotateAroundThisPoint - dla freecama, informacja o tym że się obraca
*/

}
