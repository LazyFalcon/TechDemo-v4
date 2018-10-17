#include "core.hpp"
#include "RenderDataCollector.hpp"
#include "ModelLoader.hpp"
#include "Constants.hpp"
#include "LightSource.hpp"

RenderDataCollector::tupleOfContainers RenderDataCollector::collection;
CommandArray RenderDataCollector::enviro;
CommandArray RenderDataCollector::foliage;
std::vector<LightSource*> RenderDataCollector::lights[100];
