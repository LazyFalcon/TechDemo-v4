#pragma once

namespace camera
{
/*
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
- dobrze by też było żeby przełączenie robiło się automatycznie, nie?
    -> punkt w global
    -> punkt w lokal
    -> lokal w global
    -> global w lokal
    Zmiana ta to tylko przeliczenie obecnej orientacji w eulery, więc zostają tylko:
    -> orintacja w global
    -> orintacja w lokal

Więc chyba wszytkie elementy o których myślałem mam pokryte.
Teraz rozpiszmy jak Player, Gra i appka obsługują wskaźnik i kamerę
-> app na starcie ramki zapisuje kamerę z którą będzie wyrenderowana scenka, dzięki temu to pozycja pod myszą będzie prawdziwa
-> app odbiera ruchy myszy od usera
-> gra przekazuje aktualnej kamerze input - przesunięcia lub punkt na który ma patrzeć, wokół którego ma się obracać, albo coś podobnego
-> gracz może to modyfikować -> może narzucić punkt na który ma ptrzeć kamera

-> gra/gracz może modyfikować tryby kamery, kamera powinna zostać w tym samym moiejscu po zmianie trybu

7.10
Do zrobienia w przyszłości: niech kamera będzie opisana dwoma wektorami(kierunek i góra, gdzie góra jest składową dominującą), dzięki temu będzie można łatwiej przechodzić pomiędzy stanami czy też wyliczać kierunki.
Będzie można też zaimplementować ciekawsze wersje sterowania. Np. kamera naśladująca oczy:
- może obracać się razem z głową
- albo patrzeć na konkretny punkt

A najlepiej to będzie zrobić odpowiednie implementacje kamer do obsługi specyficznych zachować: dla głowy, dla wierzyczki, dla staku
Najważniejsze żeby kamera była elastyczna i gładko przełączala się pomiędzy trybami
Bo to i tak zawsze będzie po stronie usera

I chyba też powinienem przerobić sposób w jaki rodzic wpływa na obecne położenie kamery

*/

struct ControlInput
{
    // control input
    std::optional<glm::vec4> worldPointToFocusOn; // artifical input or updated when moving camera
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
    bool targetRelativeToParent = false; // multiply target by parent rotation

    bool switchToMovementOnWorldAxes {false};
    bool moveHorizontally {false};

    // feedback to user
    bool reqiuresToFocusOnPoint = false;

    // other informations
    bool isPointerMovingFree = true; // powinien zablokować local direction
    bool isParrentFollowingPointer = false;
    bool freecam = false;

    ControlInput& focusOnCurrentPoint(bool enable=true);
    // dwie wersje: patrzymy na konkretny punkt
    // punkt co klatkę updatujemy zgodnie z przesunięciem pojazdu->patrzymy na horyzont/ punkt nieskończenie daleko


    ControlInput& followHorizontalRotation(bool enable=true);
    ControlInput& followPlane(bool enable=true);
    ControlInput& followRotation(bool enable=true);
    ControlInput& rotateWithParent(bool enable=true);
    ControlInput& smoothMovement(bool enable=true);

    void parseConfig(const Yaml& yaml);

    void resetAfterUse(){
        zoomDirection = 0;
        pointerMovement.horizontal = 0;
        pointerMovement.vertical = 0;
        pointerMovement.roll = 0;
        directionOfMovement = {};
        worldPointToZoom.reset();
    }

    void freeControl(){
        resetAfterUse();
        worldPointToFocusOn.reset();
        worldPointToFocusOnWhenSteady.reset();
        rotateAroundThisPoint.reset();
        directionToAlignCamera.reset();
    }
};

}
