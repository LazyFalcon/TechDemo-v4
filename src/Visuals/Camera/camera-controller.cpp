#include "core.hpp"
#include "Logging.hpp"
#include "camera-controller.hpp"

namespace camera
{

std::list<Controller*> listOfControllers;
Controller* activeCamera;
// todo: stack of previously active cameras?

Controller& active(){
    return activeCamera;
}

Controller::Controller(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, std::string_view type, glm::vec2 windowSize):
    yaw(0),
    pitch(0, -pi/3, pi/3),
    roll(0, -pi/2, pi/2),
    fovLimited(Camera::fov, 30*toRad, 120*toRad),
    origin(parentMatrix[3], 0.1f, 0.5f),
    rotation(glm::angleAxis(0.f, Z3), 0.1f, 0.5f)
{
    listOfControllers.push_back(this);
    if(not activeCamera) focusOn();

    setBehavior(type);

    glm::extractEulerAngleXYZ(cameraRelativeMatrix, *pitch, *yaw, *roll);
    offset = calculateEyePositionOffset(cameraRelativeMatrix);
    recalculateCamera();
}

Controller::~Controller(){
    listOfControllers.remove(this);
    if(activeCamera == this and not listOfControllers.empty()) listOfControllers.front()->focus();

}

void Controller::focusOn(){
    activeCamera = this;
}
bool Controller::hasFocus() const {
    return activeCamera == this;
}

void Controller::printDebug(){
    Camera::printDebug();
    console.log("yaw, pitch, roll:", yaw*toDeg, pitch*toDeg, roll*toDeg);
}



std::map<std::string, positionControlState> Controller::positionControlStates =
{
    {"freecam", {"freecam", &Controller::initState, &Controller::freecamPosition}},
    {"pinned", {"pinned", &Controller::initState, &Controller::pinnedPosition}},
};
std::map<std::string, rotationControlState> Controller::rotationControlStates =
{
    /* Obroty w global space, przy pomocy kątów eulera, więc niezależne od orientacji pojazdu,
        Dwa tryby obracania się: dookoła punktu, tak jak w blenderze z zachowaniem offsetu i w miejscu na podstwaie ruchu myszy po ekranie*/
    {"global-euler", {"global-euler", &Controller::initState, &Controller::global_euler}},
    /* Powyższe z tym że wektor w górę się obraca, powinno skutkować to obracaniem się na boki(roll),
        pochylenie w przód nie powinno występować albo powinno być szybko korygowane,
        w drugiej wersji kopiujemy całe pochylenie(-> nie wiem jak się to sprawdzi, przetestuj) */
    {"global-euler-copyUp", {"global-euler-copyUp", &Controller::initState, &Controller::global_euler_copyUp}},
    /* zamiast kątów eulera to kamera patrzy siena punkt pokazywany przez mysz -> może będzie działać fajniej niż euler,
        -> potrzebna jest cała implementacja pobierania takiego punktu z ekranu,, przemieszczania tego punktu i utrzymywania wskaźnika w miejscu,
        Ponadto trzeba jakoś reagować na zewnętrzne interakcje */
    {"global-focused", {"global-euler-copyUp", &Controller::initState, &Controller::global_euler_copyUp}},
    /* Kamera obraca się w przestrzeni pojazdu który śledzi, do wyboru czy aplikujemy transformację pojazdu jako bazową czy tylko modyfikujemy target kwaternion */
    {"local-euler", {"local-euler", &Controller::initState, &Controller::local_euler}},
    /* Oś right kamery utrzymujemy w jednej płaszczyźnie */
    {"local-euler-stablilized", {"local-euler-stablilized", &Controller::initState, &Controller::local_euler_stablized}},
    /* Kamera obraca siędo puntu pokazywanego przez mysz
        -> potrzebna jest cała implementacja pobierania takiego punktu z ekranu,, przemieszczania tego punktu i utrzymywania wskaźnika w miejscu,
        Ponadto trzeba jakoś reagować na zewnętrzne interakcje
        ! może być wesoło jeśli pojazd również obraca się w kierunku tego punktu, będzie więcej nierównomierności, drgań, moze też szybciej będzie się obracać */
    {"local-focused", {"local-focused", &Controller::initState, &Controller::local_focused}},
    /* Stabilizacja osi poziomej, tej w bok */
    {"local-focused-stablized", {"local-focused-stablized", &Controller::initState, &Controller::local_focused_stabilized}},

    /* Zachowanie offsetu od środka obrotu, no zawsze obraca się z kamerą,
        żeby kamera obracała się dookoła punktu innego niż origin, trzeba by jakoś zamrozić pozycję eye względem pojazdu, np. specjalnym stanem,
        offset i jego obrót sie zamraża a kamera obraca sie wokół eye
        albo moduł podaje inny pounkt obrotu, nie wokół swojego środka, np kawałek w bok*/

    /* Zachowanie myszy przy trybie focused:
        ponieważ obraca się kamera, a pozyjca myszy na ekranie się nie zmieni to kamera zgłupieje i będzie obracać się w nieskończoność
        -> dlatego też trzeba by w tej sytuacji pozycja myszy się zmieniała tak by wskaźnik pokazywał zawsze ten sam punkt świata
        -> jak zrobić obrót takiego punktu? gdy punkt jest widoczny na ekranie:
            znamy ruch kursora, wystarczy pobrać nowy punkt, ustawić kamerze, obrócić kamerę, skorygować położenie punktu
          punkt poza ekranem:
            bawimy się w raycasting
            albo zamiast puntu kazemy kamerze wyrówać się do jakiegoś wektora(punkt mocujemy na niebie) po prostu distance leci w nieskończoność a obroty lecą na podstwie kątów, nie muszą być precyzyjne
        A punkt jest po to żeby kamera patrzyła się cały czas w to samo miejsce, np. gdy pojazd rusza się na boki kamera patrzy w ten sam punkt, on też, mysz mu nie ucieknie
            ? ym, a jak będziemy się ruszać do przodu to co? miniemy ten punkt? fuck, o tym nie pomyślałem, punk jest spoko jesli robimy adsa i ktoś nas potrąci, albo idziemy na boki
            ? poza tą sytuacjąmoże być irytujący
            to moze jakiś globalny wektor? ale on sie nie będzie różnił od eulera..

            Taki focus on point kiedy ma sens? Jak celujemy w coś, tak to chyba wystarczy utrzymywać kierunek,
            czyli co? kamera ma się obrócić tak by kursor znalazł się na środku, do tego wystaczy wyznaczyć kierunek, wektor przechodzący przez kursor,
            ale jak wtedy z zachowaniem pozycji kursora? eh, trzeba by coś globalnego wymyślać
            -> na pewno potrzebny jest jakis przełącznik -> skup się na tym punkcie(** ADS, focus na wrogu, albo jak pojazd sie nie przesuwa -> przetestować **), a jak nie to patrz w tamtym kierunku,
            ! częśc z tych rzeczy to nie powinna być w ogóle odpowiedzialnosć kamery, a część zachowania pojazdu, wtedy ma to więcej sensu, zrób po prostu zachowanie:
            ! -> patrz na ten punkt i wyliczaj kierunek dynamicznie
            ! -> albo utrzymuj ten kierunek
            ! -> potrzebne zachowanie myszy wydziel gdzieś pomiędzy kamerę a playera,
            ! -> -> rozpisz tylko use case'y kiedy mieć to a kiedy to
            // ? pytanie czy local patrz na wektor to nie będzie równy zachowaniem global-euler? hmm?
            różnica powinna być taka że kamera wycentruje się na myszy, ale dla przypadku gdy celownik mamy uwiązany do środlka ekranu to faktycznie będzie to samo

        https://stackoverflow.com/questions/28530702/quaternion-based-camera
            ? co z gimbal lockiem?

            Więc może inaczej:
            - kursor lata swobodnie: to obracamy się do tego punktu, czyli to co wyżej
            - kursor jest prypięty do środka ekranu
                - euler gdzie orientację kamery wyliczamy z kątów
                - wektor gdy utrzymujemy ten sam kierunek patrzenia, niezależnie od ruchu pojazdu, obroty obracajaten wektor, może być przez to zbliżony do global-euler, chyba że w przestreni lokalnej, możliwe że będzie sie lepiej sprawował
                - punkt gdy utrzymujemy ten sam punkt na widoku, np. ads

    cały czas znajduję różne rzeczy które mi przeszkadzają :/


             */
};


glm::vec4 Controller::freecamPosition(const glm::mat4& parentTransform, ControlInput& input, float dt){
    if(input.mode == ControlInput::FreeCamMode::Around){
        // * z and x are in horizontal plane
        input.velocity += (input.move.x*Camera::right + input.move.z*Camera::at)*glm::vec4(1,1,0,0) + glm::vec4(0,0,input.move.y*.5f,0);
    }
    else {
        m_target.impulse += input.move.x*Camera::right + input.move.y*Camera::up + input.move.z*Camera::at;
    }
    origin = origin.getTarget() + input.velocity*dt;
}
glm::vec4 Controller::pinnedPosition(const glm::mat4& parentTransform, ControlInput&, float){
    return parentTransform[3];
}
glm::quat Controller::global_euler(const glm::mat4& parentTransform, ControlInput& input){
    glm::vec2 v(input.vertical*cos(-roll) - input.horizontal*sin(-roll),
                input.vertical*sin(-roll) + input.horizontal*cos(-roll));

    pitch -= (v.x * 12.f * fov)/pi;
    yaw -= (v.y * 12.f * fov)/pi;

    roll += input.roll;

    return glm::angleAxis(yaw, Z3) * glm::angleAxis(pitch, X3);
}
glm::quat Controller::global_euler_copyUp(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}
glm::quat Controller::local_euler(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}
glm::quat Controller::local_euler_stablized(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}
glm::quat Controller::local_focused(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}
glm::quat Controller::local_focused_stabilized(const glm::mat4& parentTransform, ControlInput& input){
    return glm::angleAxis(pi/3.f, Z3);
}

}
