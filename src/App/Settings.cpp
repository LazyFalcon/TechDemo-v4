#include "Settings.hpp"
#include "Logging.hpp"
#include "Yaml.hpp"

void Settings::load(){
    Yaml yaml("../data/Settings.yml");

    video.fullscreen = yaml["Video"]["Fullscreen"].boolean();
    video.size = yaml["Video"]["Size"].vec2();
    video.gamma = yaml["Video"]["Gamma"].number();
    video.camera.fov = yaml["Video"]["Camera"]["Fov"].number();
    video.camera.fov = yaml["Video"]["Camera"]["Inertia"].number();
    audio.masterVolume = yaml["Audio"]["MasterVolume"].number();
    audio.musicVolume = yaml["Audio"]["MusicVolume"].number();
    audio.effectsVolume = yaml["Audio"]["EffectsVolume"].number();
    audio.voiceVolume = yaml["Audio"]["VoiceVolume"].number();
    controls.mouseSensitivity = yaml["Controls"]["MouseSensitivity"].number();

    for(auto& it : yaml["Keybindings"])
        keybindings[it.key()] = it.string();
}

void Settings::save(){
    Yaml yaml;

    yaml["Video"]["Fullscreen"] = video.fullscreen;
    yaml["Video"]["Size"] = video.size;
    yaml["Video"]["Gamma"] = video.gamma;
    yaml["Video"]["Camera"]["Fov"] = video.camera.fov;
    yaml["Video"]["Camera"]["Inertia"] = video.camera.fov;
    yaml["Audio"]["MasterVolume"] = audio.masterVolume;
    yaml["Audio"]["MusicVolume"] = audio.musicVolume;
    yaml["Audio"]["EffectsVolume"] = audio.effectsVolume;
    yaml["Audio"]["VoiceVolume"] = audio.voiceVolume;
    yaml["Controls"]["MouseSensitivity"] = controls.mouseSensitivity;

    auto& keysRef = yaml["Keybindings"];
    for(auto& it : keybindings)
        keysRef.push(it.first, it.second);

    yaml.save("../data/Settings.yml");
    yaml.print();
}
