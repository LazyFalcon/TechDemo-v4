#pragma once
#include "Includes.hpp"

glm::vec4 colorToVec4(u32 color);
// jest zapisany abgr
u32 constexpr Color(u32 r, u32 g, u32 b, u32 a){
    return a<<24 | b<<16 | g << 8 | r;
}

glm::vec4 toVec4(u32 color);
