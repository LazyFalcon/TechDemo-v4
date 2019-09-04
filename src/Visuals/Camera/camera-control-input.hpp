#pragma once

namespace camera
{
/*

Jak rozwiązać input? w playground/player jest to zrobione na eventach, więc co ramkę nie wszytko będzie wypełniane.

Z drugiej strony różne stany kamery potrzebują różnego inputu, myslę że nie trzeba by user jakoś to różnicował i niech wypełnia wszysto

Tryby pracy:
https://stackoverflow.com/questions/28530702/quaternion-based-camera

::: nieruchomy celownik na środku, pojazd nie obraca się do celu
    - globalny wektor [waiting for evaluation]
        -> default, kamera za pojazdem
        -> ruch myszy obraca ten wektor
        input: ruch pointera + roll
    - lokalny wektor [waiting for evaluation]
        -> jesli chcemy śledzić obroty, nie trzeba kręcić myszą przy skręcaniu
        -> ruch myszy obraca ten wektor
        input: ruch pointera + roll
    - punkt [tbd]
        -> celowanie, żeby pozostać skupionym na tym samym punkcie
        -> kursor musi pozostać w punkcie
        input: punkt ze świata,
                -> wciskając RMB zapisujemy ten punkt, RMB off czyścimy flagę
                -> przy ustawieniu inSteadyFocusOnPoint robimy to też kiedy nie ma inputu
                -> trzba rozpoznać kiedy zanika input i wtedy zebrać punkt
        !!! Przy obliczaniu kierunku trzeba pamiętac o tym że kamera obraca się dookoła środka, nie w miejscu, trzeba więc obliczyć
        ! kierunek tak by się obróciła poprawnie, takie inverse zrobić

    + wpływ orientacji rodzica na rotację
        -> modyfikujemy obecną orientacje kamery, mozna dać szybkiego slerpa żeby nie bolało
        -> dla globalnego kopiuje pochylenie na boki
    + stabilizacja w poziomie
        -> zachowujemy oś right w poziomie
    Powinien działać również w przypadku przypięcia, z zerowym offsetem

::: nieruchomy celownik na środku, pojazd obraca się do kamery
    - globalny wektor [waiting for evaluation]
        -> kamera na wieżyczce, obie zjeżdżają się na ten sam punkt,
        -> ruch myszy obraca ten wektor
    - punkt [tbd]
        -> tak samo, tylko jak pojazd zacznie się poruszać to wieża pozostanie skupiona na tym samym punkcie
    + wpływ orientacji rodzica na rotację
    + stabilizacja w poziomie
    -> jakić rule na to by włączać local kiedy moduł ma opcję śledzenia kursora

::: ruchomy celownik/wskaźnik
    - globalny wektor [tbd]
        -> kursor wyznacza kierunek w przestrzeni globalnej, od oka do punktu na frustumie
           kamera wyrównuje się do tego wektora
           w widoku zza pleców ma to średni sens, ale powinno działać
           po poruszeniu kamerą trzeba tak obrócić kursor by kierunek był zachowany
        -> albo po prostu tak żeby kamera obracała się w kieunku punku znajdującego się pod myszą, i offsetować położenie punktu o przesunięcie pojazdu?
    - lokalny wektor [not]
        - bez sensu, szkoda roboty
        -> wektor ten sam, tylko trzeba
    - punkt [tbd]
        -> ads tak jak w poprzednich, mysz pokazuje punkt, kamera obraca się by punkt był w środku ekranu

    - rotacja na rządanie [tbd]
        Na przykład MMB, kursor lata sobie swobodnie, moduł też, kamera stoi w miejscu
        MMB i modyfikujemy eulery kamery, i kursor albo zachowuje pozycję w świecie albo ekranie
        - może być globalnie, wtedy pojazd może obracać się w punkt
        - moze być lokalnie, wtedy pojazd albo rusza się jak chce, albo w środek ekranu, nie może obracać się do celownika
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

23.08
? jaka jest różnica pomiędzy sterowaniem przy pomocy wektora a eulera?
    - euler jest czysto lokalny, nieuzależniony od niczego, sterowany przez zmiany położenia myszy po ekranie
    - wektory
        - w postaci wektora obracanego przez ruch myszy po ekranie - lekko bezużyteczne, eulery działają równie dobrze
        - w postaci wektora od oka do punktu w świecie, trzeba dbać o zachowanie kierunku kursora, żadnego zysku myślę, a szkoda roboty

? Jak zrobić dobrze kamerę patrzącą z oczu, np. kamera zamontowana na wieży czołgu?
    - wieża się porusza
    - kamera musi być zaimpaktowana przez wieżę
    - musi zostać zachowany kierunke patrzenia
    -> globalny wektor(euler) + stabilizacja + kopiowanie rotacji rodzica

? Kamera w statku kosmicznym?
    - pełna swoboda
    - brak góry
    -> arbitralnie wyznaczamy płaszczyznę
    -> albo górą jest góra pojazdu, wtedy kamera jest lokalna

? jednak chciałbym mieć kamerę obracającą się w kierunku pokazywanym przez mysz
    - w końcu musi się ten punkt znaleźć na środku
    -> trzeba więc jakoś modyfikować położenie punkty
    -> zostawić to do czasu aż zakodzę normalne kamery
    ! mam ! kurwa wymyśliłem! trzba zmienić sposób pokazywania położenia wskaźnika! zamiast określać jego położenie jako punkt na ekranie
    ! to można trzymać jego położenie jako odchylenie od osi głównej kamery, może to być kwaternion, albo eulery.
    ! wtedy można w łatwy sposób minimalizować uchyb kamery, a wskaxnik w końcu znajdzie się na śrdoku ekrany i przestanie się ruszać.


*/
// hmm, a jakby to uwspólnić dla wszystkich kamer? dzięki temu switch byłby bezbolesny
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
};
/*
Akcje jakie z tego wynikają:
    worldPointToFocusOn - kamera obraca się w kierunku tego punktu,
    worldPointToFocusOnWhenSteady - też, inSteadyFocusOnPoint - to jest dla usera
    worldPointToZoom do tego punktu zoomujemy, jeśli nie ma to do środka
    rotateAroundThisPoint - dla freecama, informacja o tym że się obraca
*/

}
