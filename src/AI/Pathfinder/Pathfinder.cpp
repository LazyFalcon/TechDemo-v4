#include "core.hpp"
#include "Pathfinder.hpp"
#include "ImageLoader.hpp"
#include "MapProcessing.hpp"
#include "Logging.hpp"

Waypoints Pathfinder::calculate(Waypoint from, Waypoint to){
    return {from, to};
}
void Pathfinder::preprocessMap(){
    ResultMap dataBeingProcessed {600, 600};

    pathifinderProcessAndSaveDepthMap(m_scene, m_context, dataBeingProcessed);

    // * save heightmap
    auto size = dataBeingProcessed.heightmap.size();
    std::vector<u8> simplified(size);
    for(int i=0; i<size; i++){
        simplified[i] = 255 * ((dataBeingProcessed.heightmap[i] - dataBeingProcessed.min)/(dataBeingProcessed.max-dataBeingProcessed.min));
    }

    ImageUtils::ImageParams image {};
    image.width = 600;
    image.height = 600;
    image.dataType = ImageDataType::R8;
    image.data = (void*)simplified.data();
    image.dataSize = simplified.size()*sizeof(u8);

    if(not ImageUtils::saveFromMemory("debug of heightmap", ImageDataType::R8, image)){
        error("Unable to save map");
    }
}
