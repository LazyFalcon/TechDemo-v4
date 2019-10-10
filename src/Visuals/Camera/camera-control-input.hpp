#pragma once

class Yaml;

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

enum ZoomMode : bool { FOV=false, OFFSET=true };

struct ControlInput
{
    struct {
        std::optional<glm::vec4> worldPointToFocusOn; // artifical input or updated when moving camera
        std::optional<glm::vec4> worldPointToZoom; // artifical input or updated when zooming
        std::optional<glm::vec4> worldPointToPivot; // for freecam rotations
        float zoom = 0;
        glm::vec4 velocity {};
        struct {
            float horizontal {};
            float vertical {};
            float roll {};
        } pointer;
    } input;

    struct {
        bool isFreecam = false;
        bool zoomMode = FOV;
        bool alignHorizontally = false; // target right vector always in horizontal plane so roll will be zero
        bool inLocalSpace = false;
        bool inLocalSpaceRotationOnly = false; // add parent rotation around global Z axis
        bool inLocalSpacePlane = false; // add parent inclination, copy XY plane
        bool restrictMovementToHorizontalPlane = false;
        bool useWorldInsteadLoclaAxes = false;
        float inertia = 0.f;
        float smooth = 1.f;
    } setup;
    // feedback to user
    struct {
        bool reqiuresToFocusOnPoint = false;
        bool pointerVisible = true;
        bool pointerCentered = true;
        bool pointerMovingFree = false;
    } userSetup;

    void parseConfig(const Yaml& yaml){}

    void resetAfterUse(){
        input.zoom = 0;
        input.pointer.horizontal = 0;
        input.pointer.vertical = 0;
        input.pointer.roll = 0;
        input.velocity = {};
        input.worldPointToZoom.reset();
    }

    void freeControl(){
        resetAfterUse();
        input.worldPointToFocusOn.reset();
        input.worldPointToPivot.reset();
    }
};

}
