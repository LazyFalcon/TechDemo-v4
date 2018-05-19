#pragma once
#include "common.hpp"

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
            float sensitivity;
        } camera;
    } video;

    struct Audio
    {
        int masterVolume;
        int musicVolume;
        int effectsVolume;
        int voiceVolume;
    } audio;

    struct Controls
    {
        int mouseSensitivity;
    } controls;

    struct {

    } misc;
};
