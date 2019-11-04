#pragma once

class Yaml;

// Y never will be used as axis of rotation
class Servomechanism
{
private:
    struct MinMax
    {
        MinMax(float min, float max) : min(min), max(max) {}
        float min;
        float max;
        float clamp(float x) {
            return glm::clamp(x, min, max);
        }
    };

    struct ValueTarget
    {
        ValueTarget() = default;
        ValueTarget(float v, float s) : value(v), target(v), speed(s) {}
        float value;
        float target;
        float speed;
        void setTarget(float v) {
            target = limit ? limit->clamp(v) : v;
        }
        std::optional<MinMax> limit;
    };

    // todo: later convert to glm::vec3
    struct
    {
        std::optional<ValueTarget> x;
        std::optional<ValueTarget> y;
        std::optional<ValueTarget> z;
    } axis;

    float vMax {0.05f};
    std::optional<Servomechanism::ValueTarget> retrieveAxis(const Yaml& params, int idx) const;
    float move(float diff, float dt) const;

public:
    Servomechanism(const Yaml& params);
    void updateTarget(float x, float y, float z);
    void setTarget(float x, float y, float z);
    void run(float dt);
    glm::mat4 getTransform();
};
