#pragma once
#include "IPathfinder.hpp"
#include "MapProcessing.hpp"
class Scene;
class Context;
struct ResultMap;

struct PotentialFields{
    int height = 300;
    int width = 300;
    std::vector<short> vec;
};

class Asd{
public:
    float value=0;
    int maxDist=0;
    int posX;
    int posY;

    float currentValue(int x, int y){
        float dist = std::sqrt((posX-x)*(posX-x)+(posY-y)*(posY-y));
        return value/(dist+1);
    }
};


class Pathfinder : public IPathfinder
{
private:
    Scene& m_scene;
    Context& m_context;
    ResultMap resultMap;
    int mapSize = 300;
    std::vector<Asd> semiStaticObjects;
    std::vector<Asd> trail;//repellent to  prevent stuck
    // std::vector<float> staticFieldValues;
    PotentialFields staticPotentialFields;

public:
    Pathfinder(Scene& scene, Context& context): m_scene(scene), m_context(context) {
        initializePotentialFields(300, 300, 0);
        Asd destination;
        destination.value = 20;
        destination.maxDist = -1;
        destination.posX = 250;
        destination.posY = 250;
        semiStaticObjects.push_back(destination);
    }

    void addTrail(int x, int y);

    void initializePotentialFields(int height, int width, int value=0);

    Waypoints calculate(Waypoint from, Waypoint to);

    void preprocessMap();

    float calculateFieldValue(int x, int y);

    Waypoint getNextBestField(Waypoint);

    void calculateTerrainFieldValues();

    // void saveVecAsImage(std::vector<short> vec);

    void saveVecAsImage(std::vector<short>, std::string name = "wqwqwqw");

    void saveVecAsImageNegative(std::string name="wqwqwqw");

    void generatePotentialFieldFromPoint(int x, int y, int value);

};
