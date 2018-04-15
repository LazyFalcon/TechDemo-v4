#pragma once

#include <Audio/AudioPlayerUtils.hpp>
#include <Audio/AudioPlayer.hpp>
#include <boost/core/noncopyable.hpp>

class IAudio : private boost::noncopyable
{
private:
    Audio::ContextHandler audioContext;
public:
    Audio::StreamedPlayer2D music;
    Audio::StreamedPlayer3D ambient;
    Audio::SoundPlayer2D gui;
    Audio::SoundPlayer3D effects;
    Audio::SoundPlayer3D vehicle;
    Audio::Listener listener;

    void init();
    void update(i32 dt);
};

extern unique_ptr<IAudio> audio;
