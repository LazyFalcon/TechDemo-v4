#pragma once
#include "visuals-model-definitions.hpp"

class RenderCommand
{
public:
    RenderCommand(bool isForShadows = false) : id(s_id++), isForShadows(isForShadows) {}
    static uint s_id;
    uint id;
    bool isForShadows;
    std::vector<model::Skinned*> skinned;
    std::vector<model::TrackLink*> trackLinks;
    // std::vector<ArmoredVehicleTracks*> tracks;
    // CommandArray dummy;
    // CommandArray terrain;
    // CommandArray foliage;
};