#include "core.hpp"
#include "gl_core_4_5.hpp"
#include "Sampler2D.hpp"
#include "ImageLoader.hpp"
#include "PMK.hpp"
#include "Logger.hpp"

class SamplerSingleChannelU8 : public ISampler
{
public:
    SamplerSingleChannelU8(const void *data, u32 w, u32 h) : ISampler(w, h), data((u8*)data, (u8*)data + w*h) {}
    SamplerSingleChannelU8(const std::vector<u8> &data, u32 w, u32 h) : ISampler(w, h), data(data) {}

    glm::vec4 operator()(glm::vec2 p){
        return (*this)(p.x, p.y);
    }
    glm::vec4 operator()(float x, float y){
        // double dummy;
        // x = std::modf(x, &dummy);
        // y = std::modf(y, &dummy);
        // x = (x < 0.f) ? (1-x) : (x);
        // y = (y < 0.f) ? (1-y) : (y);
        // console.error(x,y);
        // return {};
        return (*this)(u32(x * width), u32(y * height));
    }
    glm::vec4 operator()(u32 x, u32 y){
        return {
            data[x + y*width]/255.f
            , 0, 0, 0
        };
    }
private:
    std::vector<u8> data;
};

Sampler2D::Sampler2D(const std::string &filename, u32 s){
    settings = s;

    auto image = ImageUtils::loadToMemory(filename, ImageDataType::R8);
    width = image.width;
    height = image.height;
    console.info("SAMPLER", "from:", filename, width, ":", height);
    data = std::make_unique<SamplerSingleChannelU8>(image.data, image.width, image.height);
}
Sampler2D::Sampler2D(u32 s){
    settings = s;
}
glm::vec4 Sampler2D::operator()(float _x, float _y){
    _x = glm::fract(_x);
    _y = glm::fract(_y);
    _x = (_x < 0.f) ? (1-_x) : (_x);
    _y = (_y < 0.f) ? (1-_y) : (_y);
    u32 x = _x * width;
    u32 y = _y * height;
    float u = glm::fract(_x * width);
    float v = glm::fract(_y * height);
    u = (u < 0.f) ? (1-u) : (u);
    v = (v < 0.f) ? (1-v) : (v);
    if(settings & LINEAR){
        std::array<u32, 8> i;
        if(settings & CLAMP){
            i = {
                x, y,
                glm::clamp(x+1, 0u, width), y,
                x, glm::clamp(y+1, 0u, height),
                glm::clamp(x+1, 0u, width), glm::clamp(y+1, 0u, height),
            };
        }
        else if(settings & REPEAT){
            i = {
                x, y,
                (x+1)%width, y,
                x, (y+1)%height,
                (x+1)%width, (y+1)%height,
            };
        }
        return pmk::bilinearInterpolate(
            (*data)(i[0], i[1]),
            (*data)(i[2], i[3]),
            (*data)(i[4], i[5]),
            (*data)(i[6], i[7]),
            u, v
        );
    }


    return (*data)(x,y);
}
