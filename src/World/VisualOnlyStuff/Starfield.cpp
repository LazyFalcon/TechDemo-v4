#include "core.hpp"
#include "Starfield.hpp"
#include "Constants.hpp"
#include "GpuResources.hpp"

void Starfield::regenerate() {
    std::vector<Star> stars;
    auto seed = 7934;
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<u8> randomColor(170, 255);
    glm::mat3 rot = glm::orientate3(glm::vec3(0.9, 0, 0.5));

    auto genStars = [&stars, &generator, &rot, &randomColor, this](u32 count, auto vertical, auto horizontal) {
        starCount += count;
        stars.reserve(stars.size() + count);
        for(auto i = 0; i < count; i++) {
            auto h = horizontal(generator);
            auto v = vertical(generator);

            glm::vec3 pos;
            pos.x = 1.f * sin(v) * cos(h);
            pos.y = 1.f * sin(v) * sin(h);
            pos.z = 1.f * cos(v);

            pos = rot * pos;

            Color color;
            color.r = randomColor(generator);
            color.g = randomColor(generator);
            color.b = randomColor(generator);
            color.a = 200;

            float size = 1 + (i % 100 ? 0 : 1) + (i % 1000 ? 0 : 0.5);
            float brightness = 1 + (i % 100 ? 0 : 1) + (i % 1000 ? 0 : 2) + (i % 5000 ? 0 : 4) + (i % 10000 ? 0 : 6);

            stars.push_back({glm::vec4(pos, size), color, brightness});
        }
    };

    genStars(50000, std::normal_distribution<float>(hpi, 0.45), std::uniform_real_distribution<float>(0, pi * 2));
    genStars(10000, std::normal_distribution<float>(hpi, 0.1), std::uniform_real_distribution<float>(0, pi * 2));

    starsOnGpu = upload(stars);
}
// TODO: remove old VAO and it's data
VAO Starfield::upload(std::vector<Star>& stars) {
    VAO vao {};
    vao.setup();
    vao.vbo[0]
        .setup(stars)
        .attrib(0)
        .pointer_float(4, sizeof(Star), (void*)offsetof(Star, position))
        .divisor(0)
        .attrib(1)
        .pointer_color(sizeof(Star), (void*)offsetof(Star, color))
        .divisor(0)
        .attrib(2)
        .pointer_float(1, sizeof(Star), (void*)offsetof(Star, brightness))
        .divisor(0);
    vao.vboCount = 1;
    vao();
    return vao;
}
