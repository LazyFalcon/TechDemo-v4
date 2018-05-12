#include <Audio/Audio.hpp>
#include "AudioLibrary.hpp"
#include "common.hpp"

void AudioLibrary::init(){
    listener.position({0,0,0,0});
    listener.velocity({0,0,0,0});
    listener.orientation({1,0,0,0}, {0,0,1,0});
    listener.volume(1);

    music.loadDirectory("../res/audio/music/");
    clips.loadDirectory("../res/audio/effects/");
}

void AudioLibrary::update(int dt){
    music.update(dt);
    clips.update(dt);
}
