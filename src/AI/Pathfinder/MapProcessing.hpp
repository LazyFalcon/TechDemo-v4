#pragma once

/*
*   At now we need to save map for testing purphoses, so it don't need to be accurate
*   So we are collecting all objects without checking if they are obstacles,
*   Render depth othogonally from above
*   And save to file
*   Rendering don't have to be performant
*
*/
struct Scene;
class Context;

struct ResultMap
{
    int width;
    int height;
    float min;
    float max;

    std::vector<float> heightmap;
};

void pathifinderProcessAndSaveDepthMap(Scene& scene, Context& context, ResultMap& inout);
