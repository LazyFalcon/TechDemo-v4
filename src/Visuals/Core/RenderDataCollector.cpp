#include "core.hpp"
#include "RenderDataCollector.hpp"
#include "ModelLoader.hpp"
#include "Constants.hpp"
#include "LightSource.hpp"

RenderDataCollector::tupleOfContainers RenderDataCollector::collection;
CommandArray RenderDataCollector::enviro;
CommandArray RenderDataCollector::foliage;
Uniforms RenderDataCollector::uniforms;
std::vector<LightSource*> RenderDataCollector::lights[100];
