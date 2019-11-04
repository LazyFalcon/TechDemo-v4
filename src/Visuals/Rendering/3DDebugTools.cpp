#include "core.hpp"
#include "3DDebugTools.hpp"
#include "Constants.hpp"
#include "Utils.hpp"

// std::vector<Debug3DCube> debug3DCubes;
// std::vector<Debug3DCube>& getDebugCubes(){
//     return debug3DCubes;
// }

// Debug3DCube& addDebugCube(){
//     debug3DCubes.push_back({identityMatrix});
//     return debug3DCubes.back();
// }

// Debug3DCube& Debug3DCube::fromBtAabb(const btVector3 &min, const btVector3 &max, const btTransform &tr){
//     glm::vec3 s(max[0]-min[0], max[1]-min[1], max[2]-min[2]);
//     glm::vec3 centerOffset(max[0]+min[0], max[1]+min[1], max[2]+min[2]);
//     centerOffset *= 0.5f;
//     mat(convert(tr));
//     mat(glm::translate(centerOffset));
//     size(s);
//     return *this;
// }
