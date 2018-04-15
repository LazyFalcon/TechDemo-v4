#pragma once
#include "Includes.hpp"

class RandomNumberGenerator
{
public:
    RandomNumberGenerator(u32 seed) : generator(seed){}
    void reset(u32 seed){
        generator.seed(seed);
    }
    // [0,1]
    float lengthU();
    float lengthN();
    glm::vec2 onCircleN();
    glm::vec2 onCircleU();
    glm::vec3 onSphereN();
    glm::vec3 onSphereU();
    // [-1, 1]
    float normal();
    // [?, ?]
    float normal(float);
    // [-1, 1]
    float uniform();
    glm::vec4 randomOrtho(const glm::vec4& dir){
        std::uniform_real_distribution<float> urd(-1, 1);
        glm::vec4 out(urd(generator), urd(generator), urd(generator), 0);
        return glm::normalize(out - dir * glm::dot(out, dir));
    }
    // [min, max]
    float uniform(float a, float b){
        std::uniform_real_distribution<float> urd(a, b);
        return urd(generator);
    }
    int uniInt(int a, int b){
        std::uniform_int_distribution<int> urd(a, b);
        return urd(generator);
    }

private:
    std::default_random_engine generator;

    // std::uniform_int_distribution<u32> uniformUInt(0,0xffffffff);
    // std::uniform_real_distribution<float> (-1.0, 1.0);
    // std::uniform_real_distribution<float> urd01(0.0,1.0);
    // std::normal_distribution<float> nd(0.0,0.4);
};

extern RandomNumberGenerator RNG;
