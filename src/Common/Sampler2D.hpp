#pragma once

class ISampler
{
public:
    ISampler(float w, float h) : width(w), height(h) {}
    virtual ~ISampler() {}
    virtual glm::vec4 operator()(glm::vec2) = 0;
    virtual glm::vec4 operator()(float x, float y) = 0;
    virtual glm::vec4 operator()(u32 x, u32 y) = 0;

    u32 width;
    u32 height;
};
// TODO: dodac lambdę przetwarzajc output :D
class Sampler2D
{
public:
    enum Settings
    {
        NEARBY = 0b1,
        LINEAR = 0b10,
        CLAMP = 0b100,
        MIRROIR = 0b1000,
        REPEAT = 0b10000,
    };
    u32 settings {NEARBY | CLAMP};

    Sampler2D(const std::string& filename, u32 s = LINEAR | REPEAT);
    Sampler2D(u32 s = LINEAR | REPEAT);
    glm::vec4 operator()(float x, float y);

private:
    u32 width;
    u32 height;
    std::unique_ptr<ISampler> data;
};

template<typename SampleType>
class Sampler
{
public:
    Sampler() {}
    Sampler(std::vector<SampleType>& data, glm::ivec2 size) : data(std::move(data)), size(size) {}
    // [x,y] -> [0,1]
    std::function<glm::vec2(glm::vec2)> transformInput;
    std::function<SampleType(SampleType)> transformOutput;

    SampleType nearest(glm::vec2 p) {
        glm::vec2 coords(glm::floor(transformInput(p) * size));
        auto value = data[coords.x + coords.y * size.x];

        return transformOutput(value);
    }
    SampleType linear(glm::vec2 p) {
        glm::vec2 coords = transformInput(p) * size;
        auto fraction = glm::fract(coords);

        auto value = data[coords.x + coords.y * size.x];

        return transformOutput();
    }
    glm::vec2 size;
    std::vector<SampleType> data;
};
