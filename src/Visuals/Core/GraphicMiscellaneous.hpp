#pragma once

class Context;
struct BlurParameters
{
    BlurParameters(Context& c);
    glm::vec4 blurPolygon;
    std::array<float, 5> stages;
};
