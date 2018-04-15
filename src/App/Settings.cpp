#include "Settings.hpp"
#include "Logging.hpp"
#include "IMGUI.hpp"
#include "Yaml.hpp"

void Settings::load(){
    Yaml yaml("../data/Settings.yml");

    video.fullscreen = yaml["Video"]["Fullscreen"].boolean();
    video.size = yaml["Video"]["Size"].vec2();
    video.gamma = yaml["Video"]["Gamma"].number();
    video.camera.fov = yaml["Video"]["Camera"]["Fov"].number();
}

void Settings::save(){
    Yaml yaml;

    yaml["Video"]["Fullscreen"] = video.fullscreen;
    yaml["Video"]["Size"] = video.size;
    yaml["Video"]["Gamma"] = video.gamma;
    yaml["Video"]["Camera"]["Fov"] = video.camera.fov;

    yaml.save("../Settings.yml");
}
