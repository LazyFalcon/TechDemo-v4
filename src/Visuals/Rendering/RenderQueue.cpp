#include "core.hpp"
#include "RenderQueue.hpp"
#include "ModelLoader.hpp"
#include "Constants.hpp"
#include "LightSource.hpp"

RenderQueue::tupleOfContainers RenderQueue::collection;
CommandArray RenderQueue::enviro;
CommandArray RenderQueue::foliage;
std::vector<LightSource*> RenderQueue::lights[10];
