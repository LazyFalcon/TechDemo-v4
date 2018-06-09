#include "core.hpp"
#include "RNG.hpp"

RandomNumberGenerator RNG(465);

// float RNG::lengthU(){
//     return abs(uniform());
// }
// float RNG::lengthN(){
//     return abs(normal());
// }
// glm::vec2 RNG::onCircleN(){
//     return glm::normalize(glm::vec2(normal(), normal()));
// }
// glm::vec2 RNG::onCircleU(){
//     return glm::normalize(glm::vec2(uniform(), uniform()));
// }
// glm::vec3 RNG::onSphereN(){
//     return glm::normalize(glm::vec3(normal(), normal(), normal()));
// }
// glm::vec3 RNG::onSphereU(){
//     return glm::normalize(glm::vec3(uniform(), uniform(), uniform()));
// }
// float RNG::normal(){
//     return nd(generator);
// }
// float RNG::normal(float sigma){
//     std::normal_distribution<float> distribution(0.0,0.4);
//     return distribution(generator);
// }
// float RNG::uniform(){
//     return urd(generator);
// }
// float RNG::uniform(float min, float max){
//     return min + urd01(generator)*(max - min);
// }
