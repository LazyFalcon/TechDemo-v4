#pragma once
#include "CameraController.hpp"
#include "CamcontrolUtils.hpp"


/*
Opcje:
    |> dwa sposoby przybliżania: fov i dystans
    |> reagowanie na zachowanie sledzonego obiektu: żadne, kopiowanie poziomego obrotu

    |> dodatkowo chciałbym zrobić by kamerą obracać za pomocą punktu w który ma patrzeć,
       Zadaniem kontrolera byłoby patrzenie się w ten punkt.
       Obracając kamerę myszą, obracamy wektor którym celujemy, wektor trafia gdzieś w przestrzeń, w ten punk ma się kamera patrzeć

    todo: na starcie konwertuj offset na wektor i wartość skali
    ? a jak zrobić by zoomować fow i dystans niejednorodnie, ale powtarzalnie? trzeb jakąś fcjęnapisać, w głowie nie wymyślę
    ? a jakby rozbić to na komponenty? np Controller(ViewByPoint, CopyOrientation, FollowPosition)?
*/
/*
kopiowanie transformacji obiektu do którego kamera jest przypięta:
- do targetu:
    - pochylenie: kopioowanie obrotu osi Z/ zmiana up
    - obrót: kopiowanie obrotu dookoła osi Z
    - całość: kwaternion pomiędzy macieżami z tej i poprzedniej ramki
- zmiana układu współżędnych

metoda kontroli obrotu która powinna być niezależna od powyższego:
może raczej wewnętrzna reprezentacja obrientacji
- kąty eulera
- wektor kierunku
- punkt

eulery są całkiem niezależne,
wektor i punkt są zdefiniowane dwoma rzeczami: kierunkiem i wektorem w górę, oboma można manipulować niezależnie

jakie kombinacje bym chciał mieć?
- euler i śledzenie pozycji: tak jak juz jest
- euler i śledzenie pochylenia: pojazd się pochyla na bok, rusza się cała płaszczyzna, trzeba dodać dodatkowy obrót wynikający ze zmiany osi Z
- euler i śledzenie obrotu, ma to sens?: pojazd się obraca, kamera obraca się razem z nim, może byc trudne w kontroli
- euler i śledzenie całości
- euler i zmiana LOC

- wektor/punkt i śledzenie pozycji
- wektor i śledzenie pochylenia? - żeby obracać kamerę na boki; modyfikujemy wektor up
- punkt i śledzenie pochylenia? - żeby obracać kamerę na boki; modyfikujemy wektor up
- wektor i śledzenie obrotu - trzeba by obracać wektor kierunku
- punkt i śledzenie obrotu - z definicji niemożliwe, ale można zrobić jakieś wachnięcia żeby kamera nie była taka stała
- wektor/punk i zmiana LOC

-> zmiany metod zrobić jako pointery na metody wraz z jakąś metodą onEnter
-> kamera zamiast być poruszana przy pomocy funkcji, mogłaby czytać dane myszy z jakiejśc struktury, nawet żeby miała dostać do niej referencję(e, to dobre jest)
    można by tam wsadzić world pos punktu, ruchy myszy, może jakiś feedback
    -> to ma też tą zaletę że można wyjebać interfejs i zrobić stany na std::variant

Obroty eulera:
-> kontroller dostaje przesunięcie w poziomi i pionie, z tego wylicza kąty lokalnego obrotu
-> roll też skądś może wpaść

Obroty wektora:
-> jeśli dostajemy input w postaci wektora to nie jesteśmy w stanie go obracać wraz z pojazdem, możemy wtedy jedynie obracać wektor kierunku w górę
-> jesli dostajemy input w postaci eulera to obracamy ten wektor co go trzyammy w środku -> co w sumie nie różni się od kątów eulera

--- stare przemyślenia
-> dostajemy wektor z góry, to czy jest w przestrzeni lokalnej, gloalnej czy modifokowanej nie ma znaczenia,
-> dać mu jakąś opcję overwrite, żeby nadpisał target, resetLocalTransform
-> mysz jak się rusza to obraca wektor: może to być przy pomocy eulera, ale można też zrobić inne handlery

-> nie widzę jak to zrobić w sytuacji gdy mamy obrót lokalny zaaplikowany(śledzenie obrotu)? trzeba by trzymać obrót cały a nie aplikować różnicę
-> czyli z obrotu
-> todo: przemyśleć

Punkt:
-> dostajemy punkt w przestrzeni 3D, niewiele można z nim zrobić
-> jeśli nie poruszamy myszą punkt się nie przesuwa
-> jeśli obraca się kamera to mysz musi na tym punkcie pozostać

Jedna uwaga o czytaniu pozycji, przy obracaniu kamery tak by patrzya w punkt i przy swobodnej myszy(patrzymy w punkt pod myszą, zawsze albo tylko podczas ruchu)
trzeba by żeby mysz utrzymywała punkt w który została wycelowana jesli nie ma ruchu,
jeśli ruch jest to robimy coś dziwnego?
-> trzebaby jakąś bardziej zaawansowaną klasę kursora zrobić
-> pointer::moveFree(); pointer::keepPosition(); pointer::stayInCenter();

Ale abstrachując od myszy, kontroller dostaje

TODO: wszytkie te pomysły są fajne, ale może ty kurwa pomyśl czy są sensowne? rozpisz wszytkie zastosowania i pod nie układaj implementację, no debil

* zastosowania:
- kamera latająca za pojazdem, pochylająca się(lub nie) razem z nim

Dla drony:
- widok z oczu, celownik na środku albo ruchomy(musz musi czymać pozycję), dron obraca się w punkt
- widok zza pleców, celownik na środku, kamera obraca się z pojazdem, pojazd obraca się do punktu(czy to wykonalne? trzeba by robić rozróżnienie na to czy mysz się rusza)
    -> jak poruszamy myszą to ignorujemy ruchy pojazdu?
    -> pojazd celuje w punkt, kamera celuje w punkt, przy obracaniu trzeba by robić sztuczkę z kierunkiem i wyliczać obrót weektora i stamtad wyciągać punkt, nie z tego w co patrzymy bezpośrednio
- widok zza pleców,


*/
class CopyOnlyPosition2 : public CameraController
{
protected:
    using namespace Utils;
    Utils::Limits<float, periodicAngle<float>> yaw; // y, around Z axis
    Utils::Limits<float> pitch; // x, around X axis
    Utils::Limits<float> roll; // z, around Y axis
    Utils::Limits<float&> fovLimited;

    Utils::ValueFollower<glm::vec4> origin;
    Utils::ValueFollower<glm::quat, glm::quat, quaternionSlerpFunction> rotation;

    glm::vec4 calculateEyePositionOffset(const glm::vec4& cameraRelativeMatrix) const {
        // matrix describes camera relative position in space of module, so now we need to inverse camera matrix to get distance of module origin on each camera axis
        auto inv = glm::affineInverse(cameraRelativeMatrix);
        return inv[3]-glm::vec4(0,0,0,1);
    }

public:
    CopyOnlyPosition2(const glm::mat4& parentMatrix, const glm::mat4& cameraRelativeMatrix, glm::vec2 windowSize)
        : CameraController(windowSize),
        yaw(0),
        pitch(0, -pi/3, pi/3),
        roll(0, -pi/2, pi/2),
        fovLimited(fov, 30*toRad, 120*toRad),
        origin(parentMatrix[3], 0.1f, 0.5f)
        rotation(glm::quat(0), 0.1f, 0.5f)
    {
        glm::extractEulerAngleXYZ(cameraRelativeMatrix, *pitch, *yaw, *roll);
        offset = calculateEyePositionOffset(parentMatrix[3], cameraRelativeMatrix);

        recalculateCamera();
    }

    void updateMovement(float dt){
        rotation = glm::angleAxis(yaw, Z3) * glm::angleAxis(pitch, X3);
        rotation.update(dt);
        origin.update(dt);
        // update filters
        // update exposture and camera aperture
        // view depth changes with exposture :D
    }

    void recalculateCamera(){
        orientation = glm::toMat4(rotation);
        orientation[3] = rotationCenter + orientation * offset;

        Camera::recalculate();
    }

    void rotateInViewPlane(float horizontal, float vertical){
        glm::vec2 v(vertical*cos(-roll) - horizontal*sin(-roll),
                    vertical*sin(-roll) + horizontal*cos(-roll));

        pitch -= (v.x * 12.f * fov)/pi;
        yaw -= (v.y * 12.f * fov)/pi;
    }

    void roll(float angle){
        target.euler.z += angle;
    }

    void update(const glm::mat4& parentTransform, float dt){
        if(not hasFocus()) return;
        updateMovement(dt);
        recalculateCamera();
    }

    void modView(float change){
        if(mode == ZOOM_BY_FOV) modFov(change);
        else if(mode == ZOOM_BY_DISTANCE) modDistance(change);
    }

    void modFov(float scaleChange){
        // make scale change non linear - when closer steps are smaller, but in a way that is reversible
    }

    void modDistance(float scaleChange){
        // make scale change non linear - when closer steps are smaller, but in a way that is reversible
    }

    void printDebug(){
        Camera::printDebug();
        console.log("yaw, pitch, roll:", yaw*toDeg, pitch*toDeg, roll*toDeg);
    }
};
