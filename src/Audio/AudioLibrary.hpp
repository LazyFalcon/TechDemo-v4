#pragma once

#include <Audio/Audio.hpp>
#include <boost/core/noncopyable.hpp>

class AudioLibrary : private boost::noncopyable
{
public:
    Audio::ShortSoundLibrary clips;
    Audio::StreamedSoundLibrary music;
    Audio::Listener listener;

    void init();
    void update(int dt);
};
