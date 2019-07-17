#pragma once

class Yaml;

// Y never will be used as axis of rotation
class Servomechanism
{
private:
    struct MinMax
    {
        bool isSet;
        float min;
        float max;
        float clamp(float x){
            return isSet ? glm::clamp(x, min, max) : x;
        }
        bool areMinAndMaxClose(){
            return glm::epsilonEqual(min, max, 0.0001f);
        }
    };

    struct ValueTarget
    {
        float value;
        float target;
    };

    // todo: later convert to glm::vec3
    struct {
        std::optional<ValueTarget> x;
        std::optional<ValueTarget> y;
        std::optional<ValueTarget> z;
    } axis;
    struct {
        MinMax x;
        MinMax y;
        MinMax z;
    } limit {};
    float vMax {0.05f};
    std::optional<MinMax> isAxisLocked(const Yaml& params, int idx) const;
    float move(float diff, float dt) const;
public:
    Servomechanism(const Yaml& params);
    void setTarget(float x, float y, float z);
    void run(float dt);
    glm::mat4 getTransform();
};
