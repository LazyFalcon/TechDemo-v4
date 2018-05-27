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
        float masterVolume;
        float musicVolume;
        float effectsVolume;
        float voiceVolume;
    } audio;

    struct Controls
    {
        int mouseSensitivity;
    } controls;

    struct {

    } misc;

    std::map<std::string, std::string> keybindings;
};
