﻿#pragma once

namespace camera
{
class Camera;
}
struct GBufferSampler;
class Context;
class Window;

class GBufferSamplers
{
private:
    Window& window;
    Context& context;
    static std::list<GBufferSampler*> samplers;

public:
    static void registerMe(GBufferSampler*);
    static void deregisterMe(GBufferSampler*);
    void sampleGBuffer(camera::Camera& camera);
    GBufferSamplers(Window& window, Context& context) : window(window), context(context) {}
};

/**
 *  User holds instance of sample data, record registers self automaticaly during creation.
 *  Data is filled once in frame
 */
struct GBufferSampler : boost::noncopyable
{
    GBufferSampler() {
        GBufferSamplers::registerMe(this);
    }
    ~GBufferSampler() {
        GBufferSamplers::deregisterMe(this);
    }
    glm::vec4 position {0, 0, 0, 1};
    glm::vec4 normal {};
    float depth {0.5f};
    glm::vec2 samplePosition {};
    bool relativePosition {false};
};
