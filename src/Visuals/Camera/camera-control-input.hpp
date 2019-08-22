#pragma once

namespace camera
{
/*

Jak rozwiązać input? w playground/player jest to zrobione na eventach, więc co ramkę nie wszytko będzie wypełniane.

Z drugiej strony różne stany kamery potrzebują różnego inputu, myslę że nie trzeba by user jakoś to różnicował i niech wypełnia wszysto

Tryby pracy:
https://stackoverflow.com/questions/28530702/quaternion-based-camera

::: nieruchomy celownik na środku, pojazd nie obraca się do celu
    - globalny wektor
        -> default, kamera za pojazdem
        -> ruch myszy obraca ten wektor
        input: ruch pointera + roll
    - lokalny wektor
        -> jesli chcemy śledzić obroty, nie trzeba kręcić myszą przy skręcaniu
        -> ruch myszy obraca ten wektor
        input: ruch pointera + roll
    - punkt
        -> celowanie, żeby pozostać skupionym na tym samym punkcie
        -> kursor musi pozostać w punkcie
        input: punkt ze świata,
                -> wciskając RMB zapisujemy ten punkt, RMB off czyścimy flagę
                -> przy ustawieniu inSteadyFocusOnPoint robimy to też kiedy nie ma inputu
                -> trzba rozpoznać kiedy zanika input i wtedy zebrać punkt

    + wpływ orientacji rodzica na rotację
        -> modyfikujemy obecną orientacje kamery, mozna dać szybkiego slerpa żeby nie bolało
        -> dla globalnego kopiuje pochylenie na boki
    + stabilizacja w poziomie
        -> zachowujemy oś right w poziomie
    Powinien działać również w przypadku przypięcia, z zerowym offsetem

::: nieruchomy celownik na środku, pojazd obraca się do kamery
    - globalny wektor
        -> kamera na wieżyczce, obie zjeżdżają się na ten sam punkt,
        -> ruch myszy obraca ten wektor
    - punkt
        -> tak samo, tylko jak pojazd zacznie się poruszać to wieża pozostanie skupiona na tym samym punkcie
    + wpływ orientacji rodzica na rotację
    + stabilizacja w poziomie
    -> jakić rule na to by włączać local kiedy moduł ma opcję śledzenia kursora

::: ruchomy celownik
    - globalny wektor
        -> kursor wyznacza kierunek w przestrzeni globalnej, od oka do punktu na frustumie
           kamera wyrównuje się do tego wektora
           w widoku zza pleców ma to średni sens, ale powinno działać
           po poruszeniu kamerą trzeba tak obrócić kursor by kierunek był zachowany
    - lokalny wektor
        - bez sensu, szkoda roboty
        -> wektor ten sam, tylko trzeba
    - punkt

    - rotacja na rządanie
        Na przykład rpm, kursor lata sobie swobodnie, moduł też, kamera stoi w miejscu
    + wpływ orientacji rodzica na rotację
    + stabilizacja w poziomie

:: globalny wektor kierunku: euler albo dwa wektory, oba podatne na gimbal lock, wybrać który lepiej działa
    przykłady:

:: lokalny wektor kierunku: tak samo tylko target rotacja jest mnożone przez rotację rodzica

:: punkt: kamera obraca się środkiem w stronę punktu, nie działa to dobrze w sytacji gdy pojazd się porusza,
    dlatego też powinno być włączane na rządanie, np. ADS/Focus, albo gdy pojazd nie dostaje rozkazu poruszania sie(ustawienie na to)
    Dodatkowo Trzeba zrobić tak żeby punkt nie zmieniał się gdy nie poruszamy myszą, przy nierucomym celowniku jest to proste, update wtedy gdy nastapi ruch myszy
    Przy ruchomym celowniku trzeba modyfikować pozycję kursora by był cały czas w miejscu
    A tak to niech user martwi się żeby utrzymać punkt na celowniku

:: gimbal lock free? Dla statków kosmicznych, może być trochę inne.
    wersja z wektorem kierunku i wektorem w 'górę' powinna to zapewnić, tylko teraz jak efektywnie obracać takim wektorem? do rozpatrzenia
*/

struct ControlInput
{
    bool keepRightAxisHorizontal = false; // target right vector always in horizontal plane so roll will be zero
    bool parentRotationAffectCurrentRotation = false; // parent rotation messes with current rotation
    bool smoothParentRotation = false; // slerp of current rotation with parent rotation
    bool copyUpAxis = false;

    std::optional<glm::vec4> worldPointToFocusOn; // artifical input or updated when moving camera
    glm::vec4 directionToAlignCamera; // applies to target rotation
    bool directionIsInLocalSpace = false; // multiply target by parent rotation

    // powinno być gdzieś indziej
    bool inSteadyFocusOnPoint = false;
    std::optional<glm::vec4> worldPointToFocusOnWhenSteady; // updated when moving camera

    // ustawiane na starcie
    bool isPointerMovingFree = true; // powinien zablokować local direction
    bool isParrentFollowingPointer = false;

    float zoomDirection = 0;
    bool zoomByFov = false; // otherwise zoom by scalling offset
    std::optional<glm::vec4> worldPointToZoom; // artifical input or updated when zooming

    glm::vec4 directionOfMovement {};
    bool switchToMovementOnWorldAxes {false};
    bool moveHorizontally {false};
    bool freecam = false;

    std::optional<glm::vec4> rotateAroundThisPoint; // for freecam rotations
    struct {
        float horizontal;
        float vertical;
        float roll;
    } pointerMovement;
    bool reqiuresToHavePointerInTheSamePosition = false;
    void resetAfterUse(){
        zoomDirection = 0;
        pointerMovement.horizontal = 0;
        pointerMovement.vertical = 0;
        pointerMovement.roll = 0;
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
