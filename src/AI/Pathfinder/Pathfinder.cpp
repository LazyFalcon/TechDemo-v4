#include "core.hpp"
#include "Pathfinder.hpp"
#include "ImageLoader.hpp"
#include "MapProcessing.hpp"
#include "Logging.hpp"

Waypoints Pathfinder::calculate(Waypoint from, Waypoint to){
    return {from, to};
}
void Pathfinder::preprocessMap(){
    ResultMap dataBeingProcessed {1024, 1024};

    pathifinderProcessAndSaveDepthMap(m_scene, m_context, dataBeingProcessed);

    // * save heightmap
    auto size = dataBeingProcessed.heightmap.size();
    std::vector<u8> simplified(size);
    for(int i=0; i<size; i++){
        simplified[i] = 256 * ((dataBeingProcessed.heightmap[i] + dataBeingProcessed.min)/dataBeingProcessed.max);
    }

    ImageUtils::ImageParams image {};
    image.width = 1024;
    image.height = 1024;
    image.dataType = ImageDataType::R8;
    image.data = (void*)simplified.data();
    image.dataSize = simplified.size()*sizeof(u8);

    if(not ImageUtils::saveFromMemory("hmap", ImageDataType::R8, image)){
        error("Unable to save map");
    }
}
