#include "core.hpp"
#include "Pathfinder.hpp"
#include "ImageLoader.hpp"
#include "MapProcessing.hpp"
#include "Logging.hpp"
#include<iostream>
#include<cmath>


void Pathfinder::initializePotentialFields(int height, int width, int value){
    staticPotentialFields.vec = std::vector<short>(height*width);
}

void Pathfinder::saveVecAsImageNegative(std::string name){
    std::vector<short> vec(staticPotentialFields.vec.size());
    int max = 0;
    int min = 0;
    for(int i=0; i<vec.size(); i++){
        vec[i] = -staticPotentialFields.vec[i];
        if(max < vec[i]){
            max = vec[i];
        }
    }
    for(int i=0; i<vec.size(); i++){
        vec[i] = (255*255)*vec[i]/max;
    }


    int height = staticPotentialFields.height;
    int width = staticPotentialFields.width;
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
    auto staticFieldValues = staticPotentialFields.vec;
    int height = staticPotentialFields.height;
    int width = staticPotentialFields.width;
    for(int i=0; i<staticFieldValues.size(); i++){
        staticFieldValues[i] = 0;
    }

    auto &heightmap = resultMap.heightmap;

    std::vector<short> vec2(heightmap.size());
    for(int i=1; i<height-1; i++){
        for(int j=1; j<width-1; j++){
            int tmp = 0;

            int x = heightmap[i*height +j];
            tmp = abs(heightmap[(i+1)*height+j] - x);
            tmp = std::max((int)abs(heightmap[(i-1)*height+j] - x), tmp);
            tmp = std::max((int)std::abs(heightmap[i*height+j+1] - x), tmp);
            tmp = std::max((int)std::abs(heightmap[i*height+j-1] - x), tmp);

            tmp = std::max((int)abs(heightmap[(i+1)*height+j+1] - x), tmp);
            tmp = std::max((int)abs(heightmap[(i-1)*height+j-1] - x), tmp);
            tmp = std::max((int)abs(heightmap[(i-1)*height+j+1] - x), tmp);
            tmp = std::max((int)abs(heightmap[(i+1)*height+j-1] - x), tmp);

            tmp *= 10;
            vec2[i*height +j] = tmp;//////////
            if(tmp > 30){
                generatePotentialFieldFromPoint(i, j, -3);
            }
        }
    }
    saveVecAsImage(vec2, "terrain");
    saveVecAsImageNegative("staticField");
    return;
}


void Pathfinder::addTrail(int x, int y){
    Asd a;
    a.posX = x;
    a.posY = y;
    a.value = -10;
    if(trail.size()<1000){
        trail.push_back(a);
    }
}


Waypoints Pathfinder::calculate(Waypoint from, Waypoint to){
    Waypoints waypoints;
    auto &staticFieldValues = staticPotentialFields.vec;

    from.position.x = -100;//
    from.position.y = -100;//
    to.position.x = 100;//
    to.position.y = 100;//

     waypoints.push_back(from);
     Waypoint tmp = from;

    std::vector<short> vec(mapSize*mapSize);
    for(int i=0;i<mapSize*mapSize;i++){
        vec[i] = 0;
    }

  for(int i=0;tmp.position!=to.position && i<500;i++){
        tmp = getNextBestField(tmp);
        waypoints.push_back(tmp);

        vec[mapSize*((int)tmp.position.x+150) + (int)tmp.position.y+150] = 250;
        addTrail((int)tmp.position.x+150, (int)tmp.position.y+150);
    }
    for(int i=-3;i<3;i++){
        for(int j=-3;j<3;j++){
            vec[mapSize*((int)from.position.x+150+i) + (int)from.position.y+150+j] = 250;
            vec[mapSize*((int)to.position.x+150+i) + (int)to.position.y+150+j] = 250;
        }
    }

    saveVecAsImage(vec, "path");
    // return waypoints;

    //zapisanie do obrazka
    std::vector<short> vecc(300*300);
    for(int i=0;i<300;i++){
        for(int j=0;j<300;j++){
            int tmp = vecc[i*300+j];
            vecc[i*300+j] = std::min(calculateFieldValue(i,j),10.f);
            vecc[i*300+j] = std::max(vecc[i*300+j],(short)-10);

        }
    }
     saveVecAsImage(vecc, "values");

    return {from, to};
}

Waypoint Pathfinder::getNextBestField(Waypoint waypoint){
    auto position = waypoint.position;
    int x = position.x+mapSize/2;
    int y = position.y+mapSize/2;

    float maxX = 0;
    float maxY = 0;
    float maxVal = -1000;
    for(int i=-1; i<=1; i++){
        for(int j=-1; j<=1; j++){
            if((i==0 && j ==0) ||( x+i<0 || x+i>=mapSize|| y+j<0 || y+j>=mapSize)){
                continue;
            }
            float val = calculateFieldValue(x+i, y+j);
            if(maxVal < val){
                maxVal = val;
                maxX = x+i;
                maxY = y+j;
            }
        }
    }
    Waypoint p;
    p.position.x = maxX-mapSize/2;
    p.position.y = maxY-mapSize/2;
    return p;
}

float Pathfinder::calculateFieldValue(int x, int y){
    auto staticFieldValues = staticPotentialFields.vec;
    float value = 0;

    for(auto a : semiStaticObjects){
        value += a.currentValue(x, y);
    }
    for(auto a : trail){
        if(a.posX == x && a.posY == y){//////////
            value += a.currentValue(x, y);
        }
    }
    int height = staticPotentialFields.height;
    value += staticFieldValues[x*height+y];

    return value;
}



 void Pathfinder::generatePotentialFieldFromPoint(int x, int y, int value){
    auto &staticFieldValues = staticPotentialFields.vec;
    int height = resultMap.height;
    int width = resultMap.width;
    int maxDist = 7;
    for(int k=0; k<maxDist;k+=1){
        for(int i=x-k;i<x+k;i++){
            for(int j=y-k;j<y+k;j++){
                if(i<0||j<0 ||i>=height || j>=width) continue;
                int dist = std::sqrt((x-i)*(x-i)+(y-j)*(y-j));
                if(dist > maxDist){
                    continue;
                }
                int newValue = 0;
                newValue = value/(dist+1);
                staticFieldValues[i*height+j] = std::min(newValue, (int)staticFieldValues[i*height+j]);
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

    calculateTerrainFieldValues();
}
