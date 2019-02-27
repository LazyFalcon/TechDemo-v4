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
    glm::ivec2 position;

    float currentValue(glm::ivec2 pos){
        float dist = std::sqrt((pos.x-position.x)*(pos.x-position.x)+(pos.y-position.y)*(pos.y-position.y));
        return value/dist;
    }
};

template<typename DataType>
class MapSampler
{
private:
    std::vector<DataType> m_data;
public:
    int w, h;
    DataType min, max;
    void collectMinMax(){}
    MapSampler() = default;
    MapSampler(std::vector<DataType> data, int w, int h) : m_data(std::move(data)), w(w), h(h){}
    MapSampler(int w, int h, DataType t) : m_data(w*h, t), w(w), h(h){}
    MapSampler(const MapSampler<DataType>& mps) : m_data(mps), w(mps.w), h(mps.h){}
    void operator = (const MapSampler<DataType>& mps){
        m_data = mps.m_data;
        w = mps.w;
        h = mps.h;
    }
    void set(glm::ivec2 xy, DataType v){
        set(xy.x, xy.y, v);
    }
    void set(int x, int y, DataType v){
        m_data[x + y*w] = v;
    }
    const DataType& get(glm::ivec2 xy) const {
        return get(xy.x, xy.y);
    }
    const DataType& get(int x, int y) const {
        return m_data[x + y*w];
    }

    std::vector<DataType>& getData(){
        return m_data;
    }
    DataType* getRawData(){
        return m_data.data();
    }
    bool inbounds(glm::ivec2 xy){
        return inbounds(xy.x, xy.y);
    }
    bool inbounds(int x, int y){
        return x>=0 and y >=0 and x<w and y<h;
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
    std::vector<Asd> trail;
    Asd destination;
    // PotentialFields staticPotentialFields;
    MapSampler<float> staticField;
    MapSampler<float> heightField;

public:
    Pathfinder(Scene& scene, Context& context): m_scene(scene), m_context(context) {
        initializePotentialFields(300, 300, 0);
        staticField = MapSampler<float>(300, 300, 0);
        heightField = MapSampler<float>(300, 300, 0);
        destination.value = 0;
    }

    void addTrail(glm::ivec2 position);

    void initializePotentialFields(int height, int width, int value=0);

    Waypoints calculate(Waypoint from, Waypoint to);

    void preprocessMap();

    float calculateFieldValue(glm::ivec2 position);

    float calculateFieldValue_TerrainOnly(glm::ivec2 position);

    Waypoint getNextBestField(Waypoint);

    Waypoint getNextBestField2(Waypoint);

    void calculateTerrainFieldValues();

    void saveVecAsImage(std::vector<short>, std::string name = "wqwqwqw");

    void saveVecAsImageNegative(const std::string &name="wqwqwqw");

    void generatePotentialField(const glm::ivec2 &point, int value);

    void test();

    void test2();

    void test3();

};
