#include "core.hpp"
#include "SceneObject.hpp"

std::list<int> g_freeIndices;
std::vector<IntermediatePointerToObject> g_sceneObjectAddresses {{nullptr}};
