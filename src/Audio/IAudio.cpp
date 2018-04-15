#include "IAudio.hpp"
#include "Includes.hpp"

void IAudio::init(){
    listener.position({0,0,0,0});
    listener.velocity({0,0,0,0});
    listener.orientation({1,0,0,0}, {0,0,1,0});
    listener.volume(2);

    // music.loadFromDirectory("../res/audio/effects/");
    music.loadFromDirectory("../res/audio/music/");
    // ambient.loadFromDirectory("../res/audio/ambient/");
    // gui.loadFromDirectory("../res/audio/gui/");
    effects.loadFromDirectory("../res/audio/effects/");
    // vehicle.loadFromDirectory("../res/audio/vehicle/");
}

void IAudio::update(i32 dt){
    effects.update(dt);
    vehicle.update(dt);
    gui.update(dt);
}

unique_ptr<IAudio> audio;
