#include "core.hpp"
#include "Pathfinder.hpp"
#include "ImageLoader.hpp"
#include "MapProcessing.hpp"
#include "Logging.hpp"
#include<iostream>
#include<cmath>


void Pathfinder::initializePotentialFields(int height, int width, int value){

}

void Pathfinder::saveVecAsImageNegative(const std::string &name){
    // std::vector<short> vec(staticPotentialFields.vec.size());
    std::vector<short> vec(300*300);
    int max = 0;
    int min = 0;
    for(int i=0; i<vec.size(); i++){
        vec[i] = -staticField.getData()[i];
        if(max < vec[i]){
            max = vec[i];
        }
    }

    for(int i=0; i<vec.size(); i++){
        vec[i] = (255*255)*vec[i]/max;
    }


    int height = staticField.h;
    int width = staticField.w;
    ImageUtils::ImageParams image {};
    image.width = width;
    image.height = height;
    image.dataType = ImageDataType::R16;
    image.data = (void*)vec.data();
    image.dataSize = vec.size()*sizeof(u16);

    if(not ImageUtils::saveFromMemory("negativeField", ImageDataType::R16, image)){
        error("Unable to save map");
    }
}

 void Pathfinder::saveVecAsImage(std::vector<short> vec2, std::string name){
    std::vector<short> vec(vec2.size());
    float max = 0;
    float min = 0;
    for(int i=0; i<vec.size(); i++){
        if(max < vec2[i]){
            max = vec2[i];
        }
        if(min > vec2[i]){
            min = vec2[i];
        }
    }
    for(int i=0; i<vec.size(); i++){
        vec[i] = (255*255)*(vec2[i]-min)/(max-min);
    }


    ImageUtils::ImageParams image {};
    image.width = 300;
    image.height = 300;
    image.dataType = ImageDataType::R16;
    image.data = (void*)vec.data();
    image.dataSize = vec.size()*sizeof(u16);

    if(not ImageUtils::saveFromMemory(name, ImageDataType::R16, image)){
        error("Unable to save map");
    }
}

void Pathfinder::calculateTerrainFieldValues(){
    int height = staticField.h;
    int width = staticField.w;
    auto &heightmap = resultMap.heightmap;

    for(int i=1; i<height-1; i++){
        for(int j=1; j<width-1; j++){
            int tmp = 0;

            int x = heightmap[i +j*width];
            tmp = abs(heightmap[(i+1)+j*width] - x);
            tmp = std::max((int)abs(heightmap[(i-1)+j*width] - x), tmp);
            tmp = std::max((int)std::abs(heightmap[i+(j+1)*width] - x), tmp);
            tmp = std::max((int)std::abs(heightmap[i+(j-1)*width] - x), tmp);

            // int x = heightmap[i*height +j];
            // tmp = abs(heightmap[(i+1)*height+j] - x);
            // tmp = std::max((int)abs(heightmap[(i-1)*height+j] - x), tmp);
            // tmp = std::max((int)std::abs(heightmap[i*height+j+1] - x), tmp);
            // tmp = std::max((int)std::abs(heightmap[i*height+j-1] - x), tmp);

            // tmp = std::max((int)abs(heightmap[(i+1)*height+j+1] - x), tmp);
            // tmp = std::max((int)abs(heightmap[(i-1)*height+j-1] - x), tmp);
            // tmp = std::max((int)abs(heightmap[(i-1)*height+j+1] - x), tmp);
            // tmp = std::max((int)abs(heightmap[(i+1)*height+j-1] - x), tmp);

            tmp *= 10;
            // vec2[i*height +j] = tmp;//////////

            if(tmp > 5){
                generatePotentialField(glm::ivec2(i, j), -30);/////////////////
            }
        }
    }
    // saveVecAsImage(vec2, "terrain");
    // saveVecAsImageNegative("staticField");
    return;
}


void Pathfinder::addTrail(glm::ivec2 position){
    Asd a;
    a.position = position;
    a.value = -1;
    if(trail.size()>100){
        trail.erase(trail.begin());
    }
    trail.push_back(a);
}


Waypoints Pathfinder::calculate(Waypoint from, Waypoint to){
    Waypoints waypoints;

    destination.value = 100;
    destination.position = glm::ivec2(to.position.x+150, to.position.y+150);

    waypoints.push_back(from);
    Waypoint tmp = from;

  for(int i=0;tmp.position!=to.position && i<1000;i++){
        tmp = getNextBestField(tmp);
        tmp.velocity = 50;
        waypoints.push_back(tmp);

//         addTrail(glm::ivec2(tmp.position.x+150, tmp.position.y+150));
    }
    return waypoints;
    // return {from, to};
}


const std::vector<glm::ivec2> c_nextMoves {
    {0,2}, {2,2}, {2,0}, {2,-2}, {0,-2}, {-2,-2}, {-2,0}, {-2,2}
};

float Pathfinder::calculateFieldValue_TerrainOnly(glm::ivec2 position){
    return staticField.get(position);
}

Waypoint Pathfinder::getNextBestField(Waypoint waypoint){

    const auto& position = waypoint.position;
    glm::ivec2 p(position.x+mapSize/2, position.y+mapSize/2);

    float maxX = 0;
    float maxY = 0;
    float bestValue = -1;
    glm::ivec2 bestPoint{};
    for(const auto & it : c_nextMoves){
        auto point = p + it;
        if(!staticField.inbounds(point)) continue;
        float val = calculateFieldValue(point);
        val += calculateFieldValue_TerrainOnly(point+it) * 0.4f;
        if(bestValue < val){
            bestValue = val;
            bestPoint = point;
        }
    }


    Waypoint pp;
    pp.position = glm::vec4(bestPoint.x-mapSize/2, bestPoint.y-mapSize/2, heightField.get(bestPoint), 1);
    pp.direction = glm::normalize(pp.position - waypoint.position);

    pp.velocity = 50;
    return pp;
}

float Pathfinder::calculateFieldValue(glm::ivec2 position){
    float value = 0;
    value += destination.currentValue(position);

    for(auto a : semiStaticObjects){
        value += a.currentValue(position);
    }
    for(auto a : trail){
        if(a.position == position){//////////
            value += a.currentValue(position);
        }
    }
    value += staticField.get(position);
    return value;
}


 void Pathfinder::generatePotentialField(const glm::ivec2 &point, int value){
    int x = point.x;
    int y = point.y;
    int height = resultMap.height;
    int width = resultMap.width;
    int maxDist = 3;

    for(int k=0; k<maxDist; k+=1){
        for(int i=x-k; i<x+k; i++){
            for(int j=y-k;j<y+k;j++){
                if(!staticField.inbounds(point.x+i, point.y+j)) continue;
                float dist = std::sqrt((x-i)*(x-i)+(y-j)*(y-j));
                if(dist > maxDist){
                    continue;
                }
                float newValue = std::min(value/(dist+1), staticField.get(point.x+i, point.y+j));
                staticField.set(point.x+i, point.y+j, newValue);
            }
        }
    }
 }

void Pathfinder::preprocessMap(){
    ResultMap dataBeingProcessed {mapSize, mapSize};
    pathifinderProcessAndSaveDepthMap(m_scene, m_context, dataBeingProcessed);

    // * save heightmap
    auto size = dataBeingProcessed.heightmap.size();
    std::vector<u8> simplified(size);
    for(int i=0; i<size; i++){
        simplified[i] = 255 * ((dataBeingProcessed.heightmap[i] - dataBeingProcessed.min)/(dataBeingProcessed.max-dataBeingProcessed.min));
    }

    ImageUtils::ImageParams image {};
    // image.width = 600;
    // image.height = 600;
    image.width = mapSize;
    image.height = mapSize;
    image.dataType = ImageDataType::R8;
    image.data = (void*)simplified.data();
    image.dataSize = simplified.size()*sizeof(u8);

    if(not ImageUtils::saveFromMemory("debug of heightmap", ImageDataType::R8, image)){
        error("Unable to save map");
    }
    resultMap = dataBeingProcessed;
    heightField = MapSampler<float>(dataBeingProcessed.heightmap, mapSize, mapSize);
    calculateTerrainFieldValues();

    test();
    test2();
}

void Pathfinder::test(){
    // calculateTerrainFieldValues();

    int width = 300;
    int height = 300;
    int startX = 25;
    int startY = 25;
    int endX = 200;
    int endY = 250;
    int pathValue = 12;////////////
    int maxValue = 10;
    int minValue = -10;
    glm::vec4 start(startX-150, startY-150, 10, 1);
    glm::vec4 end(endX-150, endY-150, 10, 1);

    Waypoints path = calculate(Waypoint{start, glm::vec4(1,0,0,0), 50.}, Waypoint{end, glm::vec4(1,0,0,0), 50.});

    std::vector<short> vec(width*height);
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            vec[i*width + j] = calculateFieldValue(glm::ivec2(i, j));

            if(vec[i*width + j] > maxValue){
                vec[i*width + j] = maxValue;
            }
            if(vec[i*width + j] < minValue){
                vec[i*width + j] = minValue;
            }

            // if(pathValue < vec[i*width + j]){
            //     pathValue = vec[i*width + j];
            // }
        }
    }

    for(auto p : path){
        int x = p.position.x+150;
        int y = p.position.y+150;
        vec[x*width + y] = pathValue;
    }

    for(int i=-1;i<=1;i++){
        for(int j=-1;j<=1;j++){
            vec[(startX+i)*width + startY+j] = pathValue;
            vec[(endX+i)*width + endY+j] = pathValue;
        }
    }

    saveVecAsImage(vec, "field values");





}

void Pathfinder::test2(){
    // calculateTerrainFieldValues();

    int width = 300;
    int height = 300;
    int startX = 25;
    int startY = 25;
    int endX = 200;
    int endY = 250;
    int pathValue = 12;////////////
    int maxValue = 10;
    int minValue = -10;
    // glm::vec4 start(startX-150, startY-150, 10, 1);
    // glm::vec4 end(endX-150, endY-150, 10, 1);

    // Waypoints path = calculate(Waypoint{start, glm::vec4(1,0,0,0), 50.}, Waypoint{end, glm::vec4(1,0,0,0), 50.});

    std::vector<short> vec(width*height);
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            vec[i*width + j] = calculateFieldValue_TerrainOnly(glm::ivec2(i, j));

            if(vec[i*width + j] > maxValue){
                vec[i*width + j] = maxValue;
            }
            if(vec[i*width + j] < minValue){
                vec[i*width + j] = minValue;
            }

            // if(pathValue < vec[i*width + j]){
            //     pathValue = vec[i*width + j];
            // }
        }
    }

    // for(auto p : path){
    //     int x = p.position.x+150;
    //     int y = p.position.y+150;
    //     vec[x*width + y] = pathValue;
    // }

    // for(int i=-1;i<=1;i++){
    //     for(int j=-1;j<=1;j++){
    //         vec[(startX+i)*width + startY+j] = pathValue;
    //         vec[(endX+i)*width + endY+j] = pathValue;
    //     }
    // }

    saveVecAsImage(vec, "terrain values");



}
