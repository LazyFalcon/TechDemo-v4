#pragma once
#include "Includes.hpp"

class Settings
{
public:
    void load();
    void save();

    struct Video
    {
        bool fullscreen;
        glm::vec2 size;
        float gamma;
        struct {
            float fov;
            float inertia;
        } camera;
    } video;

    struct Audio
    {
        int volume;
    } audio;

    struct Controls
    {
        int mouseSensitivity;
    } controls;
};
