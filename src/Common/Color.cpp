#include "core.hpp"
#include "Color.hpp"

glm::vec4 colorToVec4(u32 color) {
    return glm::vec4(((color >> 24) & 0xFF) / 255.f, ((color >> 16) & 0xFF) / 255.f, ((color >> 8) & 0xFF) / 255.f,
                     (color & 0xFF) / 255.f);
}

glm::vec4 toVec4(u32 color) {
    return glm::vec4((color & 0xFF) / 255.f, ((color >> 8) & 0xFF) / 255.f, ((color >> 16) & 0xFF) / 255.f,
                     ((color >> 24) & 0xFF) / 255.f);
}
